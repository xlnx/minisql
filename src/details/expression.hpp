#pragma once 

#include <details/type.h>
#include <variant>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <functional>
#include <memory>

namespace minisql 
{

class ExprNode;

class UnaryExprNode;

class BinaryExprNode;

class LiteralNode;

class ColumnNode;

using Expr = std::unique_ptr<ExprNode>;

using UnaryExpr = std::unique_ptr<UnaryExprNode>;

using BinaryExpr = std::unique_ptr<BinaryExprNode>;

using Literal = std::unique_ptr<LiteralNode>;

using Column = std::unique_ptr<ColumnNode>;

class ExprNode
{
public:
	ExprNode() = default;
	virtual ~ExprNode() = default;

	virtual void write(std::ostream &os) const = 0;
};

class LiteralNode: public ExprNode
{
	friend class UnaryExprNode;
	friend class BinaryExprNode;
public:
	// LiteralNode() = default;
	template <typename T>
	LiteralNode(const T &elem);
	LiteralNode(Value &&val):
		value(std::move(val))
	{
	}

	void write(std::ostream &os) const override;
private:
	Value value;
};

template <typename ...Args>
Expr createLiteral(Args &&...args)
{
	return std::make_unique<LiteralNode>(std::forward<Args>(args)...);
}

template <>
inline LiteralNode::LiteralNode<std::string>(const std::string &elem):
	value(std::make_unique<String>(elem))
{
}
template <>
inline LiteralNode::LiteralNode<double>(const double &elem):
	value(std::make_unique<Number>(elem))
{
}
template <>
inline LiteralNode::LiteralNode<bool>(const bool &elem):
	value(std::make_unique<Bool>(elem))
{
}

class ColumnNode: public ExprNode
{
public:
	ColumnNode(const std::string &name):
		name(name)
	{
	}
	
	void write(std::ostream &os) const override;
private:
	std::string name;
};

inline Expr createColumn(const std::string &name)
{
	return std::make_unique<ColumnNode>(name);
}

class UnaryExprNode: public ExprNode
{
	friend class ExprNode;
public:
	UnaryExprNode(const std::string &op, Expr &&r):
		rhs(std::move(r)), op(op)
	{
	}

	static Literal evaluate(const std::string &op, LiteralNode *rhs)
	{
		static std::map<std::string, std::function<Value(const Value &)>> m = {
			{ "!", [](const Value &val){ return !val; } },
			{ "~", [](const Value &val){ return ~val; } }
		};
		if (m.count(op)) {
			return std::make_unique<LiteralNode>(m[op](rhs->value));
		} else {
			throw "wtf, internal error, at " + __LINE__;
		}
	}

	void write(std::ostream &os) const override;
private:
	Expr rhs;
	std::string op;
};

class BinaryExprNode: public ExprNode
{
	friend class ExprNode;
public:
	BinaryExprNode(Expr &&l, const std::string &op, Expr &&r):
		lhs(std::move(l)), rhs(std::move(r)), op(op)
	{
	}

	static Literal evaluate(LiteralNode *lhs, const std::string &op, LiteralNode *rhs)
	{
		static std::map<std::string, std::function<Value(const Value &, const Value &)>> m = {
			{ "+", [](const Value &lhs, const Value &rhs){ return lhs + rhs; } },
			{ "-", [](const Value &lhs, const Value &rhs){ return lhs - rhs; } },
			{ "*", [](const Value &lhs, const Value &rhs){ return lhs * rhs; } },
			{ "/", [](const Value &lhs, const Value &rhs){ return lhs / rhs; } },
			{ "%", [](const Value &lhs, const Value &rhs){ return lhs % rhs; } },
			{ "|", [](const Value &lhs, const Value &rhs){ return lhs | rhs; } },
			{ "&", [](const Value &lhs, const Value &rhs){ return lhs & rhs; } },
			{ "^", [](const Value &lhs, const Value &rhs){ return lhs ^ rhs; } },
			{ "<<", [](const Value &lhs, const Value &rhs){ return lhs << rhs; } },
			{ ">>", [](const Value &lhs, const Value &rhs){ return lhs >> rhs; } },
			{ "==", [](const Value &lhs, const Value &rhs){ return lhs == rhs; } },
			{ "!=", [](const Value &lhs, const Value &rhs){ return lhs != rhs; } },
			{ ">", [](const Value &lhs, const Value &rhs){ return lhs > rhs; } },
			{ "<", [](const Value &lhs, const Value &rhs){ return lhs < rhs; } },
			{ ">=", [](const Value &lhs, const Value &rhs){ return lhs >= rhs; } },
			{ "<=", [](const Value &lhs, const Value &rhs){ return lhs <= rhs; } },
			{ "&&", [](const Value &lhs, const Value &rhs){ return lhs && rhs; } },
			{ "||", [](const Value &lhs, const Value &rhs){ return lhs || rhs; } }
		};
		if (m.count(op)) {
			return std::make_unique<LiteralNode>(m[op](lhs->value, rhs->value));
		} else {
			throw "wtf, internal error, at " + __LINE__;
		}
	}

	void write(std::ostream &os) const override;
private:
	Expr lhs, rhs;
	std::string op;
};

inline Expr createExpr(Expr lhs, const std::string &op, Expr rhs)
{
	auto l = dynamic_cast<LiteralNode*>(lhs.get());
	auto r = dynamic_cast<LiteralNode*>(rhs.get());
	if (l && r) 
		return BinaryExprNode::evaluate(l, op, r);
	else 
		return std::make_unique<BinaryExprNode>(std::move(lhs), op, std::move(rhs));
}

inline Expr createExpr(const std::string &op, Expr rhs)
{
	if (auto r = dynamic_cast<LiteralNode*>(rhs.get()))
		return UnaryExprNode::evaluate(op, r);
	else
		return std::make_unique<UnaryExprNode>(op, std::move(rhs));
}

inline std::ostream & operator << (std::ostream &os, const Expr &e)
{
	e->write(os);
	return os;
}

inline void LiteralNode::write(std::ostream &os) const
{
	os << value;
}

inline void UnaryExprNode::write(std::ostream &os) const
{
	os << op << " " << rhs;
}

inline void BinaryExprNode::write(std::ostream &os) const
{
	os << lhs << " " << op << " " << rhs;
}

inline void ColumnNode::write(std::ostream &os) const
{
	os << name;
}

}