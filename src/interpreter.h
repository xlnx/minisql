#pragma once 

#include <sheet.h>
#include <string>
#include <new_parser/parser.h>
#include <new_parser/variant/variant.h>

namespace minisql
{

class Interpreter
{
	using ValueType = variant<Sheet>;
	using AstType = ast<ValueType>;
	using CharType = char;

	using LexerInitElemType = reflected_lexer_init_element<AstType, CharType>;
	static initializer<LexerInitElemType> lexerFile;
public:
	Interpreter(): lex(lexerFile) {}
	virtual ~Interpreter() = default;

	void interpret(const std::string &sql_insts);
private:
	reflected_lexer<AstType, CharType> lex;
	parser<AstType, CharType> engine();
};

}
