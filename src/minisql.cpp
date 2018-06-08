#include <interpreter/interpreter.h>
#include <debug/debug.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace debug;

// void testFile(int n)
// {
// 	auto root = IndexManager::getRoot(n);
// 	debug::print::ln(root);
// 	debug::print::ln(
// 		root[0], root[1], root[2]
// 	);
// }

int main(int argc, char **argv)
{
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