#include <interpreter/interpreter.h>
// #include <buffer/buffer.h>
#include <debug/debug.hpp>
#include <buffer/buffer_aux.h>
#include <iostream>
#include <string>

using namespace std;
using namespace minisql;
using namespace debug;

// class Row: public Buffer
// {
// BEGIN_PROPERTIES(Row)
// 	Property<char> c;
// 	Property<std::string> name;
// END_PROPERTIES
// };

// Row r;

#pragma pack(1)
struct {
	char a[30] = "Hello world!";
	int b = 2, c = 4;
	char d[10] = "Dummy";
} test;

int main(int argc, char **argv)
{
	std::ios::sync_with_stdio(false);
	// hl([&]() {
		// auto type = BufferManager::registerBufferType({SQL_CHAR(30), SQL_INT, SQL_INT, SQL_CHAR(10)});
		// auto idx = BufferManager::insert(type, reinterpret_cast<const char*>(&test));
		// auto data = BufferManager::get(type, idx);
		// BufferManager::registerBufferType({SQL_POINTER, })
		auto idx = BufferManager::insert(0, reinterpret_cast<const char*>(&test));
	// });
	return 0;
}