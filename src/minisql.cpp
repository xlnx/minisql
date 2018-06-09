#include <interpreter/interpreter.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	minisql::Interpreter engine;
	string pre = "", l;
	while (cout << (pre == "" ? ">>> " : " -> "), getline(cin, l))
	{
		if ([&]{engine.interpret(pre + l); return engine.complete();}())
			{pre = ""; cout << endl;}
		else
			{pre += l + "\n";}
	}
	return 0;
}