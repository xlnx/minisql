#include <interpreter.h>

int main(int argc, char **argv)
{
	minisql::Interpreter engine;
	engine.interpret("select 1+3/2;");
	return 0;
}