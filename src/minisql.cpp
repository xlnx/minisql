#include <interpreter/interpreter.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	minisql::Interpreter engine;
	if (argc > 1)
	{
		debug::print::ln(std::string("execfile \"") + argv[1] + "\";");
		engine.interpret(std::string("execfile \"") + argv[1] + "\";");
	}
	else
	{
		string pre = "", l;
		while (cout << (pre == "" ? ">>> " : " -> "), getline(cin, l))
		{
			if ([&]{engine.interpret(pre + l); return engine.complete();}())
				{pre = ""; cout << endl;}
			else
				{pre += l + "\n";}
		}
	}
	return 0;
}