#include <interpreter.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	minisql::Interpreter engine;
	string l;
	while (cout << ">>> ", getline(cin, l))
	{
		engine.interpret(l);
		cout << endl;
	}
	return 0;
}