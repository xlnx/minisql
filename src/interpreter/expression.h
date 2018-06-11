#pragma once 

#include <interpreter/type.h>
#include <variant>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <functional>
#include <memory>

namespace minisql 
{

namespace __interpret
{

class ExprNode;

class UnaryExprNode;

class BinaryExprNode;

class IsExprNode;

class LiteralNode;

class ColumnNode;

using Expr = std::unique_ptr<ExprNode>;

using UnaryExpr = std::unique_ptr<UnaryExprNode>;

using BinaryExpr = std::unique_ptr<BinaryExprNode>;

using Literal = std::unique_ptr<LiteralNode>;

using Column = std::unique_ptr<ColumnNode>;

class ExprNode
{
	friend class API;
public:
	ExprNode() = default;
	virtual ~ExprNode() = default;

	virtual void write(std::ostream &os) const = 0;
};

template <typename T>
struct LiteralType
{
};

template <>
struct LiteralType<std::string>
{
	using type = String;
};

template <>
struct LiteralType<double>
{
	using type = Number;
};

template <>
struct LiteralType<bool>
{
	using type = Bool;
};

class LiteralNode: public ExprNode
{
	friend class UnaryExprNode;
	friend class BinaryExprNode;
	friend class IsExprNode;
	friend class API;
public:
	// LiteralNode() = default;
	LiteralNode():
		value(std::make_unique<Null>())
	{
	}
	template <typename T>
	LiteralNode(const T &elem):
		value(std::make_unique<typename LiteralType<T>::type>(elem))
	{
	}
	LiteralNode(Value &&val):
		value(std::move(val))
	{
	}

	void write(std::ostream &os) const override;
	template <typename T>
	bool is() const { return dynamic_cast<const T*>(value.get()); }
	Value &getValue() { return value; }
public:
	Value value;
};

template <typename ...Args>
Expr createLiteral(Args &&...args)
{
	return std::make_unique<LiteralNode>(std::forward<Args>(args)...);
}

class ColumnNode: public ExprNode
{
	friend class API;
public:
	ColumnNode(const std::string &name):
		name(name)
	{
	}

	const std::string &getName() const { return name; }
	
	void write(std::ostream &os) const override;
public:
	std::string name;
};

inline Expr createColumn(const std::string &name)
{
	return std::make_unique<ColumnNode>(name);
}

enum IsExprType
{
	IsTrue,
	IsFalse,
	IsUnknown
};

class IsExprNode: public ExprNode
{
	friend class ExprNode;
	friend class API;
public:
	IsExprNode(Expr &&l, bool inv, IsExprType isType):
		lhs(std::move(l)), inv(inv), isType(isType)
	{
	}

	static Literal evaluate(LiteralNode *lhs, bool inv, IsExprType isType)
	{
		if (lhs->template is<Null>())
			return std::make_unique<LiteralNode>(
				inv && (isType == IsTrue || isType == IsFalse) || !inv && (isType == IsUnknown));
		else
			return std::make_unique<LiteralNode>(
					inv && (isType == IsUnknown || isType == IsFalse && lhs->value->toBool()) || 
				!inv && (isType == IsTrue && lhs->value->toBool()));
	}

	void write(std::ostream &os) const override;
public:
	Expr lhs;
	bool inv;
	IsExprType isType;
};

class UnaryExprNode: public ExprNode
{
	friend class ExprNode;
	friend class API;
public:
	UnaryExprNode(const std::string &op, Expr &&r):
		rhs(std::move(r)), op(op), callback(lookup[op])
	{
	}

	static Literal evaluate(const std::string &op, LiteralNode *rhs)
	{
		return evaluate(lookup[op], rhs);
	}
	static Literal evaluate(const std::function<
		Value(const Value &)> &callback, LiteralNode *rhs)
	{
		return std::make_unique<LiteralNode>(callback(rhs->value));
	}

	void write(std::ostream &os) const override;
public:
	Expr rhs;
	std::string op;
	const std::function<Value(const Value &)> &callback;

	static std::map<std::string, std::function<Value(const Value &)>> lookup;
};

class BinaryExprNode: public ExprNode
{
	friend class ExprNode;
	friend class API;
public:
	BinaryExprNode(Expr &&l, const std::string &op, Expr &&r):
		lhs(std::move(l)), rhs(std::move(r)), op(op), callback(lookup[op])
	{
	}

	static Literal evaluate(LiteralNode *lhs, const std::string &op, LiteralNode *rhs)
	{
		return evaluate(lhs, lookup[op], rhs);
	}
	static Literal evaluate(LiteralNode *lhs, const std::function<
		Value(const Value &, const Value &)> &callback, LiteralNode *rhs)
	{
		return std::make_unique<LiteralNode>(callback(lhs->value, rhs->value));
	}

	void write(std::ostream &os) const override;
public:
	Expr lhs, rhs;
	std::string op;
	const std::function<Value(const Value &, const Value &)> &callback;

	static std::map<std::string, std::function<Value(const Value &, const Value &)>> lookup;
};

Expr createExpr(Expr &&lhs, const std::string &op, Expr &&rhs);

Expr createExpr(Expr &&lhs, bool inv, IsExprType isType);

Expr createExpr(const std::string &op, Expr &&rhs);

inline std::ostream & operator << (std::ostream &os, const Expr &e)
{
	e->write(os);
	return os;
}

}

using __interpret::Expr;

}