#pragma once

#include <type_traits>
#include <initializer_list>
#include <iomanip>
#include <typeinfo>
#include <functional>
#include <chrono>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <memory>

#ifdef __GNUC__
#include <cxxabi.h>
#endif
#ifdef WIN32
// to make windows happy for printing 
#include <windows.h>
#endif

#if (__cplusplus >= 201703L)
#include <variant>
#endif

#define lambda(...) \
[=](__VA_ARGS__) mutable

namespace debug
{

namespace __helper
{

template <bool X, bool ...Args>
struct aux_and: std::integral_constant<bool, X && aux_and<Args...>::value>
{
};

template <bool X>
struct aux_and<X>: std::integral_constant<bool, true>
{
};

class color_manip
{
	bool hl;

#	ifdef WIN32
	static WORD &attr() { static WORD attr; return attr; }
	static void *handle() { static auto hwnd = GetStdHandle(STD_OUTPUT_HANDLE); return hwnd; }
#	endif

	static bool &hle() { static bool e = false; return e; }
public:
	color_manip():
		hl(color_manip::hle() = !color_manip::hle())
	{
	}
	color_manip(bool hl):
		hl(hl)
	{
	}

	friend std::ostream &operator << (std::ostream &os, const color_manip &color)
	{
		if (color.hl) {
#			ifdef WIN32
			// pretty print for WIN32
			// support cmd and powershell
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (GetConsoleScreenBufferInfo(color_manip::handle(), &csbi))
			color_manip::attr() = csbi.wAttributes;
			os.flush();
			SetConsoleTextAttribute(color_manip::handle(), FOREGROUND_RED | FOREGROUND_INTENSITY |
				FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_RED);
#			else
			os << "\033[1m\033[41;37m";
#			endif
		} else {
#			ifdef WIN32
			os.flush();
			SetConsoleTextAttribute(color_manip::handle(), color_manip::attr());
#			else
			os << "\033[0m";
#			endif
		}
		return os;
	}
};

} // helper

inline __helper::color_manip hl()
{
	static bool hl = false;
	return __helper::color_manip(hl = !hl);
}

inline void hl(const std::function<void()> &f, std::ostream &os = std::cout)
{
	os << __helper::color_manip();
		f();
	os << __helper::color_manip();
}

inline double time(const std::function<void()> &f)
{
	auto start = std::chrono::high_resolution_clock::now();
		f();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double>(end - start).count();
}

template <typename T>
inline std::string demangle(const T &e)
{
	std::string mangled_name = typeid(e).name();
#	ifdef __GNUC__
	int status;
	auto demangled_name = abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status);
	if (demangled_name) {
		mangled_name = demangled_name;
		std::free(demangled_name);
	}
#	endif
	return mangled_name;
}

template <typename T>
inline std::string demangle()
{
	std::string mangled_name = typeid(T).name();
#	ifdef __GNUC__
	int status;
	auto demangled_name = abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status);
	if (demangled_name) {
		mangled_name = demangled_name;
		std::free(demangled_name);
	}
#	endif
	return mangled_name;
}

template <typename T>
class alias
{
	T val;
public:
	alias() = default;
	~alias() = default;

	template <typename ...Types>
	alias(Types &&...args): val(std::forward<Types>(args)...) {}

	operator T &() { return val; }
	operator const T &() const { return val; }
};

namespace strutil
{

template <typename T, typename = typename
	std::enable_if<
		!std::is_same< typename
			std::remove_reference< typename
				std::remove_const<T>::type
			>::type,
			std::string
		>::value
	>::type
>
inline std::string operator + (const std::string &s, const T &e)
{
	std::ostringstream os; os << e; return s + os.str();
}

template <typename T, typename = typename
	std::enable_if<
		!std::is_same< typename
			std::remove_reference< typename
				std::remove_const<T>::type
			>::type,
			std::string
		>::value
	>::type
>
inline std::string operator + (const T &e, const std::string &s)
{
	std::ostringstream os; os << e; return os.str() + s;
}

inline std::string operator ""_s (const char s[], std::size_t n)
{
	return s;
} 

} // strutil

namespace visualize
{
	
template <typename ...Types>
inline std::ostream &operator << (std::ostream &os, const std::map<Types...> &m)
{
	os << "{";
	if (m.size())
	{
		os << "\n  " << m.begin()->first << ": " << m.begin()->second;
		auto it = m.begin();
		for (++it; it != m.end(); ++it)
		{
			os << ", \n  " << it->first << ": " << it->second;
		}
	}
	os << "\n}"; return os;
}

template <typename T, typename ...Types, int X = 0, typename = typename 
	std::enable_if<!std::is_aggregate<T>::value>::type>
inline std::ostream &operator << (std::ostream &os, const std::vector<T, Types...> &m)
{
	os << "[ ";
	if (m.size())
	{
		os << *m.begin();
		auto it = m.begin();
		for (++it; it != m.end(); ++it)
		{
			os << ", " << *it;
		}
	}
	os << " ]"; return os;
}

template <typename T, typename ...Types, unsigned X = 0, typename = typename 
	std::enable_if<std::is_aggregate<T>::value>::type>
inline std::ostream &operator << (std::ostream &os, const std::vector<T, Types...> &m)
{
	os << "[";
	if (m.size())
	{
		os << "\n  " << *m.begin();
		auto it = m.begin();
		for (++it; it != m.end(); ++it)
		{
			os << ", \n  " << *it;
		}
	}
	os << "\n]"; return os;
}

} // visualize;

#if (__cplusplus >= 201703L)

namespace __variant_aux
{

template <typename T, typename ...Types>
struct printer
{
	template <typename ...Args>
	static void print(std::ostream &os, const std::variant<Args...> &v)
	{
		using namespace visualize;
		if (auto ptr = std::get_if<T>(&v)) os << *ptr; else printer<Types...>::print(os, v);
	}
};

template <typename ...Types>
struct printer<std::nullptr_t, Types...>
{
	template <typename ...Args>
	static void print(std::ostream &os, const std::variant<Args...> &v)
	{
		using namespace visualize;
		if (auto ptr = std::get_if<std::nullptr_t>(&v)) os << "null"; else printer<Types...>::print(os, v);
	}
};

template <typename T>
struct printer<T>
{
	template <typename ...Args>
	static void print(std::ostream &os, const std::variant<Args...> &v)
	{
		using namespace visualize;
		if (auto ptr = std::get_if<T>(&v)) os << *ptr; else throw std::bad_variant_access();
	}
};

template <>
struct printer<std::nullptr_t>
{
	template <typename ...Args>
	static void print(std::ostream &os, const std::variant<Args...> &v)
	{
		using namespace visualize;
		if (auto ptr = std::get_if<std::nullptr_t>(&v)) os << "null"; else throw std::bad_variant_access();
	}
};

template <typename T, typename ...Types>
struct typegetter
{
	template <typename ...Args>
	static std::string type(const std::variant<Args...> &v)
	{
		if (std::holds_alternative<T>(v)) return demangle<T>(); else return typegetter<Types...>::type(v);
	}
};

template <typename T>
struct typegetter<T>
{
	template <typename ...Args>
	static std::string type(const std::variant<Args...> &v)
	{
		if (std::holds_alternative<T>(v)) return demangle<T>(); else throw std::bad_variant_access();
	}
};

}

template <typename T, typename ...Types>
inline std::ostream &operator << (std::ostream &os, const std::variant<T, Types...> &v)
{
	__variant_aux::printer<T, Types...>::print(os, v); return os;
}

#endif

#if (__cplusplus >= 201703L)

template <typename T, typename ...Args>
inline std::string type(const std::variant<T, Args...> &v)
{
	return __variant_aux::typegetter<T, Args...>::type(v);
}

class var;

namespace __var_aux
{

using var_type = std::variant<
	double,
	std::string,
	std::vector<var>
>;

}

class var: public alias<std::shared_ptr<__var_aux::var_type>>
{
	__var_aux::var_type &get() { return *this->operator std::shared_ptr<__var_aux::var_type> &(); }
	const __var_aux::var_type &get() const { return *this->operator const std::shared_ptr<__var_aux::var_type> &(); }
public:
	var() = default;
	var(std::nullptr_t):
		var() {}
	var(const std::initializer_list<var> &l):
		var(std::vector<var>(l)) {}
	var(const var&) = default;
	var(var &&) = default;
	var &operator = (const var &other) = default;
	var &operator = (var &&other) = default;
	~var() = default;

	var(int x):
		alias<std::shared_ptr<__var_aux::var_type>>(
			std::make_shared<__var_aux::var_type>(double(x)))
		{}
	template <typename T, typename ...Types, typename = typename 
		std::enable_if<!std::is_same<T, var>::value>::type>
	var(T &&arg, Types &&...args): 
		alias<std::shared_ptr<__var_aux::var_type>>(
			std::make_shared<__var_aux::var_type>(
				std::forward<T>(arg), std::forward<Types>(args)...)) 
		{}

	var &operator [] (std::size_t index)
		{ auto &vec = std::get<std::vector<var>>(get()); 
			if (vec.size() <= index) vec.resize(index + 1); return vec[index]; }

	operator __var_aux::var_type &() 
		{ return get(); }
	operator const __var_aux::var_type &() 
		{ return get(); }

	const var operator + (const var &other) const
	{
		auto &l = this->operator const std::shared_ptr<__var_aux::var_type> &();
		auto &r = other.operator const std::shared_ptr<__var_aux::var_type> &();
		if (std::holds_alternative<double>(*l) && std::holds_alternative<double>(*r))
		{
			return this->val() + other.val();
		}
		else
		{
			return this->str() + other.str();
		}
	}
	// const var operator - (const var &other) const
	// {

	// }
	// const var operator * (const var &other) const
	// {

	// }
	// const var operator / (const var &other) const
	// {

	// }

	var &operator += (const var &other) 
		{ *this = *this + other; }

	operator std::string() const
		{ return str(); }
	operator double () const
		{ return val(); }
private:
	std::string str() const
	{
		using namespace strutil;
		auto &l = this->operator const std::shared_ptr<__var_aux::var_type> &();
		if (std::holds_alternative<std::string>(*l))
		{
			return std::get<std::string>(*l);
		}
		else if (std::holds_alternative<double>(*l))
		{
			return std::string("") + std::get<double>(*l);
		}
		else 
		{
			return "[Object]";
		}
	}
	double val() const
	{
		auto &l = this->operator const std::shared_ptr<__var_aux::var_type> &();
		if (std::holds_alternative<double>(*l))
		{
			return std::get<double>(*l);
		}
		if (std::holds_alternative<std::string>(*l))
		{
			std::istringstream is(std::get<std::string>(*l));
			double val; is >> val; return val;
		}
		else
		{
			return 0;
		}
	}
};

using let = var;

constexpr auto null = nullptr;

inline std::string type(const var &v)
{
	auto &p = v.operator const std::shared_ptr<__var_aux::var_type> &();
	if (p == nullptr) return "object";
	auto &val = *p;
	if (std::holds_alternative<double>(val)) return "number";
	// if (std::holds_alternative<bool>(val)) return "boolean";
	if (std::holds_alternative<std::string>(val)) return "string";
	return "object";
}

inline std::ostream &operator << (std::ostream &os, const var &v)
{
	using namespace visualize;
	auto &p = v.operator const std::shared_ptr<__var_aux::var_type> &();
	if (p == nullptr) return os << "null";
	auto &val = *p;
	if (std::holds_alternative<std::string>(val)) return os << "\"" << *p << "\"";
	return os << *p;
}

#endif

namespace print
{

template <typename ...Args>
inline void ln(Args &&...args)
{
	using namespace visualize;
	int os[] = { (std::cout << args << " ", 0)... };
	std::cout << std::endl; 
}

template <typename _CharT, typename _Traits>
inline void state(std::basic_istream<_CharT, _Traits> &s)
{
	ln("eof:", (int)s.eof(), "fail:", (int)s.fail(), "bad:", (int)s.bad(), "g:", (int)s.tellg());
}

template <typename _CharT, typename _Traits>
inline void state(std::basic_ostream<_CharT, _Traits> &s)
{
	ln("eof:", (int)s.eof(), "fail:", (int)s.fail(), "bad:", (int)s.bad(), "p:", (int)s.tellp());
}


template <typename _CharT, typename _Traits>
inline void state(std::basic_iostream<_CharT, _Traits> &s)
{
	ln("eof:", (int)s.eof(), "fail:", (int)s.fail(), "bad:", (int)s.bad(), "g:", (int)s.tellg(), "p:", (int)s.tellp());
}

inline double time(const std::function<void()> &f)
{
	auto sec = ::debug::time(f);
	std::cout << "Time: " << sec << " sec" << std::endl;
	return sec;
}

template <typename T>
inline void demangle(const T &e)
{
	std::cout << debug::demangle(e) << std::endl;
}

template <typename T>
inline void mem(const T &obj)
{
	auto begin = reinterpret_cast<const unsigned char*>(&obj);
	auto end = begin + sizeof(T);
	std::cout << "Layout: "; 
	auto mangled_name = typeid(T).name();
#	ifdef __GNUC__
	int status;
	auto demangled_name = abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
	if (demangled_name) {
		std::cout << demangled_name;
		std::free(demangled_name);
	} else 
#	endif
	std::cout << mangled_name;
	std::cout << " [" << &obj << "]" << std::endl;
	auto ptr = reinterpret_cast<const unsigned char *>(
			reinterpret_cast<unsigned long long>(begin) >> 4 << 4);
	auto eptr = reinterpret_cast<const unsigned char *>(
			((reinterpret_cast<unsigned long long>(end) >> 4) + 1) << 4);
	while (ptr != eptr) {
		std::cout << "[" << (void*)ptr << "]   ";
		for (int i = 0; i != 8 && ptr != eptr; ++i) {
			if (ptr < begin) std::cout << "   "; else
			if (ptr >= end) std::cout << "   "; else
			std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(*ptr) << " ";
			ptr++;
		}
		std::cout << "  ";
		for (int i = 0; i != 8 && ptr != eptr; ++i) {
			if (ptr < begin) std::cout << "   "; else
			if (ptr >= end) std::cout << "   "; else
			std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(*ptr) << " ";
			ptr++;
		}
		std::cout << std::endl;
	}
	std::cout << std::dec;
}

} // print

} // debug

