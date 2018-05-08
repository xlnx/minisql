#include <details/expression.h>

namespace minisql
{

const Expr Expr::none;

Expr::Value Expr::evaluate()
{
	return Value();
}

std::function<auto (const Expr &) -> Expr>
	Expr::operator [] (const std::string &op) const
{
	return [this, op](const Expr &rhs) -> Expr {
		if (&rhs == &Expr::none)
		{
			return Expr(new UnaryExpr(op, *this));
		}
		else
		{
			return Expr(new BinaryExpr(*this, op, rhs));
		}
	};
}

void Expr::dispose()
{
	if (value.is<UnaryExpr*>())
	{
		auto p = value.get<UnaryExpr*>();
		p->rhs.dispose();
		delete p;
	}
	else if (value.is<BinaryExpr*>())
	{
		auto p = value.get<BinaryExpr*>();
		p->lhs.dispose();
		p->rhs.dispose();
		delete p;
	}
	else if (!value.is<Value>())
	{
		throw "internal error: unknown expression value type.";
	}
}

}