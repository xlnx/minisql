#include <interpreter/interpreter.h>
#include <interpreter/expression.h>
#include <iostream>

namespace minisql
{

namespace __interpret
{

using ValueType = Interpreter::ValueType;
using AstType = Interpreter::AstType;
using CharType = Interpreter::CharType;

using reflect = make_reflect<AstType>;

template <typename ... Args>
	initializer<reflected_lexer_init_element<AstType, CharType>>
		getLexerFile(Args &&... args)
{
	return { reflected_lexer_init_element<AstType, CharType>(std::forward<Args>(args))... };
}

initializer<Interpreter::LexerInitElemType> Interpreter::lexerFile = getLexerFile(
#include <lang/lexer.hs>
);

initializer<Interpreter::ParserInitElemType> Interpreter::parserFile = {
#include <lang/parser.hs>
};

void Interpreter::interpret(const std::string &sql_insts)
{
	try
	{
		engine.parse(sql_insts.c_str());
		isComplete = true;
	}
	catch (parser<AstType>::exception_type e)
	{
		if (isComplete = e.reason != "source incomplete")
		{
			std::cout << "minisql-interpreter" << e.what() << std::endl;
		}
	}
	catch (InterpretError e)
	{
		std::cout << "minisql-interpreter: error: " << e.what() << std::endl;
		isComplete = true;
	}
	catch (std::string f)
	{
		std::ifstream is(f);
		if (is)
		{
			API::doPrint = false;
			auto sec = debug::time([&,this]
			{
				std::string pre = "", l;
				while (getline(is, l))
				{
					if ([&,this]{interpret(pre + l); return complete();}())
						{pre = "";}
					else
						{pre += l + "\n";}
				}
			});
			std::ostringstream os;
			os << std::dec << sec;
			debug::print::ln("Query OK", "(" + os.str() + " sec)");
			API::doPrint = true;
		}
		else
		{
			std::cout << "minisql-interpreter: error: file not found: '" << f << "'." << std::endl;
		}
	}
}
	
}

}