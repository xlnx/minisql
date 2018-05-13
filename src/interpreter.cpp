#include <interpreter.h>
#include <details/expression.h>
#include <iostream>

namespace minisql
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
}
	
}

