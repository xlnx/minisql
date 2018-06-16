#include <interpreter/expression.h>
#include <regex>

namespace minisql
{

namespace __interpret
{

static Value like(const Value &lhs, const Value &rhs)
{
	std::string l = lhs->toString();
	std::string expr = "^" + std::string(rhs->toString()) + "$";
	auto pos = std::string::npos;
	while ((pos = expr.find('%')) != std::string::npos)
	{
		expr.replace(pos, 1, ".*");
	}
	while ((pos = expr.find('_')) != std::string::npos)
	{
		expr.replace(pos, 1, ".");
	}
	auto r = std::regex(expr, ::std::regex::nosubs);
	return std::make_unique<Bool>(std::regex_match(l.begin(), l.end(), r));
}

std::map<std::string, std::function<Value(const Value &, const Value &)>> 
	BinaryExprNode::lookup = {
		{ "+", operator + },
		{ "-", [](const Value &l, const Value &r){ return l - r; } },
		{ "*", operator * },
		{ "/", operator / },
		{ "%", operator % },
		{ "|", operator | },
		{ "&", operator & },
		{ "^", operator ^ },
		{ "<<", [](const Value &l, const Value &r){ return l << r; } },
		{ ">>", operator >> },
	{ "==", operator == },
	{ "!=", operator != },
	{ ">", operator > },
	{ "<", operator < },
	{ ">=", operator >= },
	{ "<=", operator <= },
		{ "&&", operator && },
		{ "||", operator || },
		{ "like", like }
};

std::map<std::string, std::function<Value(const Value &)>> 
	UnaryExprNode::lookup = {
	{ "!", operator ! },
	{ "~", operator ~ },
	{ "-", [](const Value &e) { return -e; } }
};

Expr createExpr(Expr &&lhs, const std::string &op, Expr &&rhs)
{
	auto l = dynamic_cast<LiteralNode*>(lhs.get());
	auto r = dynamic_cast<LiteralNode*>(rhs.get());
	if (l && l->template is<Null>() || r && r->template is<Null>())
		return createLiteral(); 
	if (l && r) 
		return BinaryExprNode::evaluate(l, op, r);
	else 
		return std::make_unique<BinaryExprNode>(std::move(lhs), op, std::move(rhs));
}

Expr createExpr(Expr &&lhs, bool inv, IsExprType isType)
{
	if (auto l = dynamic_cast<LiteralNode*>(lhs.get()))
		return IsExprNode::evaluate(l, inv, isType);
	else
		return std::make_unique<IsExprNode>(std::move(lhs), inv, isType);
}

Expr createExpr(const std::string &op, Expr &&rhs)
{
	if (auto r = dynamic_cast<LiteralNode*>(rhs.get()))
		if (r->template is<Null>())
			return createLiteral();
		else
			return UnaryExprNode::evaluate(op, r);
	else
		return std::make_unique<UnaryExprNode>(op, std::move(rhs));
}

void IsExprNode::write(std::ostream &os) const 
{
	os << lhs << " is " << (inv ? "not ": "") << 
		(isType == IsTrue ? "true" : isType == IsFalse ? "false" : "unknown");
}

void LiteralNode::write(std::ostream &os) const
{
	os << value;
}

void UnaryExprNode::write(std::ostream &os) const
{
	os << op << " " << rhs;
}

void BinaryExprNode::write(std::ostream &os) const
{
	os << lhs << " " << op << " " << rhs;
}

void ColumnNode::write(std::ostream &os) const
{
	os << name;
}

}

}