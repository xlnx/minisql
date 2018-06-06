#include <interpreter/interpreter.h>
// #include <buffer/buffer.h>
#include <debug/debug.hpp>
#include <buffer/buffer_aux.h>
#include <buffer/item.h>
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
	char a[30] = "Hello shit!";
	int b = 2, c = 4;
	char d[10] = "Non Dummy";
} test;

int main(int argc, char **argv)
{
	std::ios::sync_with_stdio(false);
	// hl([&]() {
		// auto type = BufferManager::registerBufferType({SQL_CHAR(30), SQL_INT, SQL_INT, SQL_CHAR(10)});
		// auto idx = BufferManager::insert(type, reinterpret_cast<const char*>(&test));
		// auto data = BufferManager::get(type, idx);
		// BufferManager::registerBufferType({SQL_POINTER, })
		
		// for (int i = 0; i != 10000; ++i) {
		// 	auto idx = BufferManager::insert(0, reinterpret_cast<const char*>(&test));
		// 	print::ln(idx);
		// }
		
		// print::mem(*reinterpret_cast<char(*)[sizeof(test)]>(BufferManager::read(0, 0)));
		// BufferManager::write(0, 0, reinterpret_cast<const char*>(&test));
		// print::mem(*reinterpret_cast<char(*)[sizeof(test)]>(BufferManager::read(0, 0)));

		Item item(0);
		debug::print::ln(item.typeName());
		debug::print::ln(item[0].typeName());
	// });
	return 0;
}