#include <interpreter.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	minisql::Interpreter engine;
	string prev = "", l;
	while (cout << (prev == "" ? ">>> " : " -> "), getline(cin, l))
	{
		engine.interpret(prev + l);
		if (engine.complete())
		{
			prev = "";
			cout << endl;
		}
		else
		{
			prev += l + "\n";
		}
	}
	return 0;
}