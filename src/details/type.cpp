#include <details/type.h>

namespace minisql
{

Value Type::operator + (const Value &other) const
	{ return toNumber() + other->toNumber(); }
Value Type::operator - (const Value &other) const
	{ return toNumber() - other->toNumber(); }
Value Type::operator * (const Value &other) const
	{ return toNumber() * other->toNumber(); }
Value Type::operator / (const Value &other) const
	{ return toNumber() / other->toNumber(); }
Value Type::operator % (const Value &other) const
	{ return toNumber() % other->toNumber(); }
Value Type::operator | (const Value &other) const
	{ return toNumber() | other->toNumber(); }
Value Type::operator & (const Value &other) const
	{ return toNumber() & other->toNumber(); }
Value Type::operator ^ (const Value &other) const
	{ return toNumber() ^ other->toNumber(); }
Value Type::operator << (const Value &other) const
	{ return toNumber() << other->toNumber(); }
Value Type::operator >> (const Value &other) const
	{ return toNumber() >> other->toNumber(); }

Value Type::operator ! () const
	{ return !toNumber(); }
Value Type::operator ~ () const
	{ return ~toNumber(); }

Value Type::operator == (const Value &other) const
	{ return toNumber() == other->toNumber(); }
Value Type::operator != (const Value &other) const
	{ return toNumber() != other->toNumber(); }
Value Type::operator > (const Value &other) const
	{ return toNumber() > other->toNumber(); }
Value Type::operator < (const Value &other) const
	{ return toNumber() < other->toNumber(); }
Value Type::operator >= (const Value &other) const
	{ return toNumber() >= other->toNumber(); }
Value Type::operator <= (const Value &other) const
	{ return toNumber() <= other->toNumber(); }

Value Type::operator && (const Value &other) const
	{ return toBool() && other->toBool(); }
Value Type::operator || (const Value &other) const
	{ return toBool() || other->toBool(); }

Number String::toNumber() const
{
	std::istringstream is(value);
	double d;
	is >> d;
	return d;
}
Bool String::toBool() const
{
	std::istringstream is(value);
	double d;
	is >> d;
	return d != 0;
}
String String::toString() const
{
	return *this;
}

Number Number::toNumber() const
{
	return *this;
}
Bool Number::toBool() const
{
	return value != 0;
}
String Number::toString() const
{
	std::ostringstream os;
	os << (value ? 1 : 0);
	return os.str();
}

Number Bool::toNumber() const
{
	return value ? 1 : 0;
}
Bool Bool::toBool() const
{
	return *this;
}
String Bool::toString() const
{
	std::ostringstream os;
	os << (value ? 1 : 0);
	return os.str();
}

}