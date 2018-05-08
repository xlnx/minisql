#pragma once 

#include <new_parser/variant/variant.h>
#include <sstream>
#include <string>
#include <functional>

namespace minisql 
{

class Expr;
class UnaryExpr;
class BinaryExpr;

// enum Type {
	// 	Integer = 0x10,
	// 	Fixed = 0x11,
	// 	Float = 0x12,
	// 	Date = 0x20,
	// 	String = 0x30,
	// 	Set = 0x40,
	// };

class Expr
{
public:
	using Value = variant<int, float, std::string>;
	Expr() = default;
public:
	Expr(Value value):
		value(value)
	{
	}
	Expr(UnaryExpr *unary):
		value(unary)
	{
	}
	Expr(BinaryExpr *binary):
		value(binary)
	{
	}
	
	Value evaluate();

	std::function<auto (const Expr &) -> Expr>
		operator [] (const std::string &op) const;

	void dispose();
private:
	variant<Value, UnaryExpr*, BinaryExpr*> value;
public:
	static const Expr none;
};

class UnaryExpr
{
	friend class Expr;
public:
	UnaryExpr(const std::string &op, const Expr &rhs):
		op(op), rhs(rhs)
	{
	}
private:
	std::string op;
	Expr rhs;
};

class BinaryExpr
{
	friend class Expr;
public:
	BinaryExpr(const Expr &lhs, const std::string &op, const Expr &rhs):
		op(op), lhs(lhs), rhs(rhs)
	{
	}
private:
	std::string op;
	Expr lhs;
	Expr rhs;
};

}