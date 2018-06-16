#pragma once

#include <variant>
#include <string>
#include <memory>
#include <iostream>
#include <sstream>

namespace minisql
{

namespace __interpret
{

class Type;

class String;

class Number;

class Bool;

using Value = std::unique_ptr<Type>;

class Type
{
public:
	Type() = default;
	virtual ~Type() = default;

	virtual String toString() const = 0;
	virtual Bool toBool() const = 0;
	virtual Number toNumber() const = 0;

	virtual void write(std::ostream &os) const = 0;

	virtual Value operator + (const Value &other) const;
	virtual Value operator - (const Value &other) const;
	virtual Value operator * (const Value &other) const;
	virtual Value operator / (const Value &other) const;
	virtual Value operator % (const Value &other) const;
	virtual Value operator | (const Value &other) const;
	virtual Value operator & (const Value &other) const;
	virtual Value operator ^ (const Value &other) const;
	virtual Value operator << (const Value &other) const;
	virtual Value operator >> (const Value &other) const;

	virtual Value operator ! () const;
	virtual Value operator ~ () const;
	virtual Value operator - () const;

	virtual Value operator == (const Value &other) const;
	virtual Value operator != (const Value &other) const;
	virtual Value operator > (const Value &other) const;
	virtual Value operator < (const Value &other) const;
	virtual Value operator >= (const Value &other) const;
	virtual Value operator <= (const Value &other) const;

	virtual Value operator && (const Value &other) const;
	virtual Value operator || (const Value &other) const;
};

class Null: public Type
{
public:
	Null() = default;

	Number toNumber() const override;
	Bool toBool() const override;
	String toString() const override;
	void write(std::ostream &os) const override;
};

inline Value operator + (const Value &lhs, const Value &rhs) { return *lhs + rhs; }
inline Value operator - (const Value &lhs, const Value &rhs) { return *lhs - rhs; }
inline Value operator * (const Value &lhs, const Value &rhs) { return *lhs * rhs; }
inline Value operator / (const Value &lhs, const Value &rhs) { return *lhs / rhs; }
inline Value operator % (const Value &lhs, const Value &rhs) { return *lhs % rhs; }
inline Value operator | (const Value &lhs, const Value &rhs) { return *lhs | rhs; }
inline Value operator & (const Value &lhs, const Value &rhs) { return *lhs & rhs; }
inline Value operator ^ (const Value &lhs, const Value &rhs) { return *lhs ^ rhs; }
inline Value operator << (const Value &lhs, const Value &rhs) { return *lhs << rhs; }
inline Value operator >> (const Value &lhs, const Value &rhs) { return *lhs >> rhs; }

inline Value operator ! (const Value &rhs) { return !*rhs; }
inline Value operator ~ (const Value &rhs) { return ~*rhs; }
inline Value operator - (const Value &rhs) { return -*rhs; }

inline Value operator == (const Value &lhs, const Value &rhs) { return *lhs == rhs; }
inline Value operator != (const Value &lhs, const Value &rhs) { return *lhs != rhs; }
inline Value operator > (const Value &lhs, const Value &rhs) { return *lhs > rhs; }
inline Value operator < (const Value &lhs, const Value &rhs) { return *lhs < rhs; }
inline Value operator >= (const Value &lhs, const Value &rhs) { return *lhs >= rhs; }
inline Value operator <= (const Value &lhs, const Value &rhs) { return *lhs <= rhs; }

inline Value operator && (const Value &lhs, const Value &rhs) { return *lhs && rhs; }
inline Value operator || (const Value &lhs, const Value &rhs) { return *lhs || rhs; }

class String: public Type
{
	friend class Number;
public:
	String(const std::string &s = ""):
		value(s)
	{
	}

	Number toNumber() const override;
	Bool toBool() const override;
	String toString() const override;
	void write(std::ostream &os) const override;

	operator std::string() const
		{ return value; }
private:
	std::string value;
};

class Bool: public Type
{
public:
	Bool(bool val = false):
		value(val)
	{
	}

	Number toNumber() const override;
	Bool toBool() const override;
	String toString() const override;
	void write(std::ostream &os) const override;

	Value operator && (const Bool &other) const
		{ return std::make_unique<Bool>(value && other.value); }
	Value operator || (const Bool &other) const
		{ return std::make_unique<Bool>(value || other.value); }

	operator bool() const
		{ return value; }
private:
	bool value;
};

class Number: public Type
{
	friend Value Type::operator / (const Value &other) const;
public:
	Number(double d = 0):
		value(d)
	{
	}

	Number toNumber() const override;
	Bool toBool() const override;
	String toString() const override;
	void write(std::ostream &os) const override;

	Value operator + (const Number &other) const
		{ return std::make_unique<Number>(value + other.value); }
	Value operator - (const Number &other) const
		{ return std::make_unique<Number>(value - other.value); }
	Value operator * (const Number &other) const
		{ return std::make_unique<Number>(value * other.value); }
	Value operator / (const Number &other) const
		{ return std::make_unique<Number>(value / other.value); }
	Value operator % (const Number &other) const
		{ return std::make_unique<Number>(double((long long)value % (long long)other.value)); }
	Value operator | (const Number &other) const
		{ return std::make_unique<Number>(double((long long)value | (long long)other.value)); }
	Value operator & (const Number &other) const
		{ return std::make_unique<Number>(double((long long)value & (long long)other.value)); }
	Value operator ^ (const Number &other) const
		{ return std::make_unique<Number>(double((long long)value ^ (long long)other.value)); }
	Value operator << (const Number &other) const
		{ return std::make_unique<Number>(double((long long)value << (long long)other.value)); }
	Value operator >> (const Number &other) const
		{ return std::make_unique<Number>(double((long long)value >> (long long)other.value)); }
	Value operator ~ () const
		{ return std::make_unique<Number>(double(~(long long)value)); }
	Value operator ! () const
		{ return std::make_unique<Number>(!value); }
	Value operator - () const
		{ return std::make_unique<Number>(-value); }

	virtual Value operator == (const Number &other) const
		{ return std::make_unique<Number>(value == other.value); }
	virtual Value operator != (const Number &other) const
		{ return std::make_unique<Number>(value != other.value); }
	virtual Value operator > (const Number &other) const
		{ return std::make_unique<Number>(value > other.value); }
	virtual Value operator < (const Number &other) const
		{ return std::make_unique<Number>(value < other.value); }
	virtual Value operator >= (const Number &other) const
		{ return std::make_unique<Number>(value >= other.value); }
	virtual Value operator <= (const Number &other) const
		{ return std::make_unique<Number>(value <= other.value); }

	operator double() const
		{ return value; }
private:
	double value;
};

inline std::ostream & operator << (std::ostream &os, const Value &e)
{
	e->write(os);
	return os;
}

}

using __interpret::Value;

}