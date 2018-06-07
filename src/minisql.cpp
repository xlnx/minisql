#include <interpreter/interpreter.h>
// #include <buffer/buffer.h>
#include <debug/debug.hpp>
#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
// #include <index/bplusTree.h>
#include <iostream>
#include <string>

using namespace std;
using namespace minisql;
using namespace debug;

void testFile(int n)
{
	auto root = IndexManager::getRoot(n);
	debug::print::ln(root);
	debug::print::ln(
		root[0], root[1], root[2]
	);
	//debug::print::ln(root[1]);
	// debug::print::ln(root[2]);
}

int main(int argc, char **argv)
{
	// BufferManager::registerBufferType({SQL_CHAR(30), SQL_INT, SQL_INT, SQL_CHAR(10)});
	// BufferManager::registerBufferType({SQL_POINTER_NODE, SQL_POINTER_DATA, SQL_POINTER_NODE}, 0);
	minisql::Interpreter engine;
	string pre = "", l;
	while (cout << (pre == "" ? ">>> " : " -> "), getline(cin, l))
	{
		engine.interpret(pre + l);
		if (engine.complete())
		{
			pre = "";
			cout << endl;
		}
		else
		{
			pre += l + "\n";
		}
	}
	return 0;
}