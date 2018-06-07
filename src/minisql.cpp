#include <interpreter/interpreter.h>
// #include <buffer/buffer.h>
#include <debug/debug.hpp>
#include <buffer/buffer_aux.h>
#include <index/bplusTree.h>
#include <iostream>
#include <string>

using namespace std;
using namespace minisql;
using namespace debug;

int main(int argc, char **argv)
{
	// BufferManager::registerBufferType({SQL_CHAR(30), SQL_INT, SQL_INT, SQL_CHAR(10)});
	// BufferManager::registerBufferType({SQL_POINTER_NODE, SQL_POINTER_DATA, SQL_POINTER_NODE}, 0);

	BPlusTree btree;
	btree.init();

		
	// for (int i = 0; i != 100; ++i) {
	// 	auto item = BufferManager::insertItem(0, {"Hello world!", 2, 4, "Non Dummy"});
	// }
	return 0;
}