#pragma once

#include <type_traits>
#include <iomanip>
#include <typeinfo>
#include <functional>
#include <chrono>
#include <iostream>
#include <fstream>

#ifdef __GNUC__
#include <cxxabi.h>
#endif
#ifdef WIN32
// to make windows happy for printing 
#include <windows.h>
#endif

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

}

inline __helper::color_manip hl(bool hl)
{
	return __helper::color_manip(hl);
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
	auto mangled_name = typeid(e).name();
#	ifdef __GNUC__
	int status;
	auto demangled_name = abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
	if (demangled_name) {
		mangled_name = demangled_name;
		std::free(demangled_name);
	}
#	endif
	return mangled_name;
}

namespace print
{

template <typename ...Args>
inline void ln(Args &&...args)
{
	volatile int os[] = { (std::cout << args << " ", 0)... };
	std::cout << std::endl; 
}

template <typename _CharT, typename _Traits>
inline void state(std::basic_istream<_CharT, _Traits> &s)
{
	ln("eof:", s.eof(), "fail:", s.fail(), "bad:", s.bad(), "g:", s.tellg());
}

template <typename _CharT, typename _Traits>
inline void state(std::basic_ostream<_CharT, _Traits> &s)
{
	ln("eof:", s.eof(), "fail:", s.fail(), "bad:", s.bad(), "p:", s.tellp());
}


template <typename _CharT, typename _Traits>
inline void state(std::basic_iostream<_CharT, _Traits> &s)
{
	ln("eof:", s.eof(), "fail:", s.fail(), "bad:", s.bad(), "g:", s.tellg(), "p:", s.tellp());
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

}

} // debug

