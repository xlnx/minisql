#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
#include <sstream>
#include <iostream>
#include <cstring>

namespace minisql
{

namespace __buffer
{

const std::string Attribute::typeName() const
{
	return BufferManager::getTypeName(BufferManager::files[item.type]->elems[index]);
}

std::ostream &operator << (std::ostream &os, const Attribute &attr)
{
	os << "attr[" << attr.item.type << ":" << attr.item.index << ":" << attr.index << "]{" << attr.typeName() << ":";
	if (auto str = std::get_if<std::string>(&attr.value))
	{ os << *str; }
	else if (auto val = std::get_if<std::string>(&attr.value))
	{ os << *val; }
	else if (auto val = std::get_if<float>(&attr.value))
	{ os << *val; }
	else if (auto val = std::get_if<Item>(&attr.value))
	{ os << *val; }
	else
	{ os << "null"; }
	return os << "}";
}

const std::string Item::typeName() const
{
	return BufferManager::demangle(type);
}

const Attribute Item::attr (std::size_t attrno) const 
{
	return Attribute(*this, attrno);
}

std::ostream &operator << (std::ostream &os, const Item &item)
{
	os << "item[" << item.type << ":" << item.index << "]"; 
	return os;
}

}

}