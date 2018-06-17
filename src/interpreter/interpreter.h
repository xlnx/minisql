#pragma once 

#include <interpreter/expression.h>
#include <interpreter/interpreter_aux.h>
#include <new_parser/parser.h>
#include <variant>
#include <string>
#include <vector>

namespace minisql
{

namespace __interpret
{

class Interpreter
{
public:
	using ValueType = std::variant<
		Expr, 
		double, 
		std::string, 
		Flag,
		Value,
		BufferElem,
		TableAttribute,
		std::vector<Value>,
		std::vector<std::string>,
		std::vector<TableAttribute>,
		std::pair<bool, IsExprType>
	>;
	using AstType = ast<ValueType>;
	using CharType = char;
	
	using reflect = make_reflect<AstType>;
private:
	using LexerInitElemType = reflected_lexer_init_element<AstType, CharType>;
	using ParserInitElemType = parser_init_element<AstType>;

	static initializer<LexerInitElemType> lexerFile;
	static initializer<ParserInitElemType> parserFile;
public:
	Interpreter(): lex(lexerFile), engine(lex, parserFile) {}
	virtual ~Interpreter() = default;

	void interpret(const std::string &sql_insts);
	bool complete() const { return isComplete; }
private:
	reflected_lexer<AstType, CharType> lex;
	parser<AstType, CharType> engine;

	bool isComplete;
};

}

using __interpret::Interpreter;

}