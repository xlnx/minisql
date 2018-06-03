#pragma once

#include <buffer/property.h>
#include <map>
#include <vector>

namespace minisql
{

#define BEGIN_PROPERTIES(T) \
private:\
	T(int) {}\
public:\
	T() { for (auto p = &__m_property_begin + 1; p != &__m_property_end; ++p) \
			*p = typename minisql::__buffer_property__shared::Mark(\
					reinterpret_cast<decltype(p)>(static_cast<Buffer*>(this)) - p); }\
private:\
	typename minisql::__buffer_property__shared::Mark __m_property_begin; \
	virtual const typename minisql::__buffer_property__shared::Mark *getPropertyBegin() const override { return &__m_property_begin; } \
	virtual const typename minisql::__buffer_property__shared::List &getPropertyList() const override \
	{ static typename minisql::__buffer_property__shared::List e = [](){ \
			T c(0); auto begin = &c.__m_property_begin + 1; auto end = &c.__m_property_end; \
			typename minisql::__buffer_property__shared::List v; \
			decltype(""-"") off = 0;\
			for (auto p = begin; p != end; ++p) { v.emplace_back(minisql::__property_traits::serializers[int(*p)], \
				off); off += minisql::__property_traits::serializers[int(*p)].size; }\
		return v; }(); return e; }\
public:

#define END_PROPERTIES \
private:\
	typename minisql::__buffer_property__shared::Mark __m_property_end;\
public:

class Buffer;

namespace __buffer_property__shared
{
	using Mark = char;
	using List = std::vector<std::pair<__property_traits::SerializerLookup, decltype(""-"")>>;
}

template <typename T, typename = void>//typename 
	//std::enable_if<__property_traits::isValidPropertyType<T>()>::type>
class Property final
{
	friend class Buffer;
	typename __buffer_property__shared::Mark serializerId = __property_traits::getSerializerId<T>();
public:
	Property()
		{ static_assert(sizeof(*this) == sizeof(typename __buffer_property__shared::Mark)); }
public:
	const T &val();
	Property &operator = (const T &value);
	Property &operator = (T &&value);
	template <typename X, typename = typename
		std::enable_if<std::is_constructible<T, X>::value>::type>
	Property &operator = (const X &value);
	template <typename X, typename = typename
		std::enable_if<std::is_constructible<T, X>::value>::type>
	Property &operator = (X &&value);
};

class BufferManager;

class Buffer
{
	friend class BufferManager;
	char *data;
public:
	Buffer() = default;
	virtual ~Buffer() = default;

	char *getData();
	virtual const typename __buffer_property__shared::List &getPropertyList() const = 0;
	virtual const typename __buffer_property__shared::Mark *getPropertyBegin() const = 0;
};

class BufferManager
{
public:
	static void load(Buffer *buf)
	{
		static const char *name = "test.dat";
		auto &l = buf->getPropertyList();
		auto len = 0;
		for (auto &e: l)
		{
			len += e.first.size;
		}
		buf->data = new char [len];
		std::fstream file(name, std::ios::binary | std::ios::in | std::ios::out);
		file.seekp(9, std::ios::beg);
		for (auto &e: l)
		{
			e.first.construct(buf->data + e.second);
			e.first.deserialize(file, buf->data + e.second);
		}
	}
};

template <typename T, typename U>
inline const T &Property<T, U>::val()
{
	auto buf = reinterpret_cast<Buffer*>((decltype(""-""))(serializerId) + &serializerId);
	auto id = &serializerId - buf->getPropertyBegin() - 1;
	return *reinterpret_cast<const T*>(buf->getData() + buf->getPropertyList()[id].second);
}

template <typename T, typename U>
inline Property<T, U> &Property<T, U>::operator = (const T &value)
{
	auto buf = reinterpret_cast<Buffer*>((decltype(""-""))(serializerId) + &serializerId);
	auto id = &serializerId - buf->getPropertyBegin() - 1;
	*reinterpret_cast<T*>(buf->getData() + buf->getPropertyList()[id].second) = value;
	return *this;
}
template <typename T, typename U>
inline Property<T, U> &Property<T, U>::operator = (T &&value)
{
	auto buf = reinterpret_cast<Buffer*>((decltype(""-""))(serializerId) + &serializerId);
	auto id = &serializerId - buf->getPropertyBegin() - 1;
	*reinterpret_cast<T*>(buf->getData() + buf->getPropertyList()[id].second) = std::forward<T>(value);
	return *this;
}

template <typename T, typename U>
template <typename X, typename>
Property<T, U> &Property<T, U>::operator = (const X &value)
{
	auto buf = reinterpret_cast<Buffer*>((decltype(""-""))(serializerId) + &serializerId);
	auto id = &serializerId - buf->getPropertyBegin() - 1;
	*reinterpret_cast<T*>(buf->getData() + buf->getPropertyList()[id].second) = T(value);
	return *this;
}
template <typename T, typename U>
template <typename X, typename>
Property<T, U> &Property<T, U>::operator = (X &&value)
{
	auto buf = reinterpret_cast<Buffer*>((decltype(""-""))(serializerId) + &serializerId);
	auto id = &serializerId - buf->getPropertyBegin() - 1;
	*reinterpret_cast<T*>(buf->getData() + buf->getPropertyList()[id].second) = T(std::forward<X>(value));
	return *this;
}

inline char *Buffer::getData()
{
	if (!data)
	{
		BufferManager::load(this);
	}
	return data; 
}

}