#include <interpreter/interpreter.h>
#include <buffer/buffer.h>
#include <iostream>
#include <string>

using namespace std;
using namespace minisql;

class Row: Buffer
{
BEGIN_PROPERTIES(Row)
	Property<char> c;
	Property<std::string> name;
END_PROPERTIES
};

Row r;

int main(int argc, char **argv)
{
	std::cout << r.name.val() << std::endl;
	r.name = "Koishi";
	std::cout << r.name.val() << std::endl;
	return 0;
}