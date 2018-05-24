#pragma once

#include <type_traits>
#include <string>
#include <new>
#include <fstream>

namespace minisql
{

namespace __property_traits
{

template <typename T, typename = typename std::enable_if<std::is_pod<T>::value>::type>
struct SerializerPod
{
	static void serialize(std::fstream &f, const char *data) { f.write(data, sizeof(T)); }
	static void deserialize(std::fstream &f, char *data) { f.read(data, sizeof(T)); }
};
template <typename T>
struct Serializer;

template <typename T>
constexpr inline bool isValidPropertyType()
{
	if (!std::is_pod<T>::value) return std::is_constructible<Serializer<T>>::value; return true;
}

template <>
struct Serializer<std::string>
{
	static void serialize(std::fstream &f, const char *data) 
	{
		auto str = reinterpret_cast<const std::string*>(data);
		unsigned len = str->length(); 
		f.write(reinterpret_cast<const char*>(&len), sizeof(len));
		f.write(str->c_str(), len);
	}
	static void deserialize(std::fstream &f, char *data)
	{
		auto str = reinterpret_cast<std::string*>(data);
		unsigned len;
		f.read(reinterpret_cast<char*>(&len), sizeof(len));
		if (len != 0)
		{
			auto cs = new char[len]; f.read(cs, len); *str = std::string(cs, len); delete cs;
		}
		else
		{
			*str = "";
		}
	}
};

template <typename T>
constexpr inline void construct(char *data)
{
	new (data) T();
}
constexpr inline void constructPod(char *data)
{
}

template <typename T>
constexpr inline void destroy(char *data)
{
	auto o = reinterpret_cast<T*>(data); o->~T();
}
constexpr inline void destroyPod(char *data)
{
}

struct SerializerLookup {
	void (*serialize)(std::fstream &, const char *);
	void (*deserialize)(std::fstream &, char *);
	void (*construct)(char *);
	void (*destroy)(char *);
	decltype (""-"") size;

	constexpr bool operator == (const SerializerLookup &other) const
	{
		return serialize == other.serialize && deserialize == other.deserialize &&
			construct == other.construct && destroy == other.destroy && size == other.size;
	}
};

template <typename T>
constexpr inline SerializerLookup genSerializer() 
{
	if constexpr (std::is_pod<T>::value)
	{
		return {
			SerializerPod<T>::serialize,
			SerializerPod<T>::deserialize,
			constructPod,
			destroyPod,
			sizeof(T)
		};
	}
	else
	{
		return {
			Serializer<T>::serialize,
			Serializer<T>::deserialize,
			construct<T>,
			destroy<T>,
			sizeof(T)
		};
	}
}

constexpr SerializerLookup serializers[] = { 
	genSerializer<char>(),
	genSerializer<unsigned char>(),
	genSerializer<short>(),
	genSerializer<unsigned short>(),
	genSerializer<int>(),
	genSerializer<unsigned int>(),
	genSerializer<long long>(),
	genSerializer<unsigned long long>(),
	genSerializer<std::string>(),
};

template <typename T>
constexpr inline int getSerializerId()
{
	for (int i = 0; i != sizeof(serializers) / sizeof(serializers[0]); ++i)
	{
		if (serializers[i] == genSerializer<T>()) return i;
	}
	return -1;
}

}

}