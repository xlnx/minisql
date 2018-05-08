#pragma once 

#include <sheet.h>
#include <details/expression.h>
#include <new_parser/parser.h>
#include <new_parser/variant/variant.h>
#include <string>

namespace minisql
{

class Interpreter
{
public:
	using ValueType = variant<Sheet, Expr, int>;
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
private:
	reflected_lexer<AstType, CharType> lex;
	parser<AstType, CharType> engine;
};

}
