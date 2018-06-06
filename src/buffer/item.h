#pragma once

#include <buffer/buffer_def.h>
#include <utility>

namespace minisql
{

namespace __buffer
{

class Item;

class Attribute
{
	friend class Item;
private:
	BufferElem type;
	char *data;

	Attribute(BufferElem type, char *data): type(type), data(data) {}
public:
	bool operator < (const Attribute &) const;
	bool operator <= (const Attribute &) const;
	bool operator > (const Attribute &) const;
	bool operator >= (const Attribute &) const;
	bool operator == (const Attribute &) const;
	bool operator != (const Attribute &) const;

	std::string typeName() const;
};

class Item
{
private:
	BufferType type; 
	DataIndex index;
public:
	Item(BufferType type): type(type) {}

	Attribute operator [] (std::size_t attrno);

	std::string typeName() const;
};

}

using __buffer::Item;
using __buffer::Attribute;

}