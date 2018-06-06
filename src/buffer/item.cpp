#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
#include <sstream>
#include <iostream>
#include <cstring>

namespace minisql
{

namespace __buffer
{

bool Attribute::operator < (const Attribute &) const
{
	return false;
}
bool Attribute::operator <= (const Attribute &) const
{
	return false;
}
bool Attribute::operator > (const Attribute &) const
{
	return false;
}
bool Attribute::operator >= (const Attribute &) const
{
	return false;
}
bool Attribute::operator == (const Attribute &) const
{
	return false;
}
bool Attribute::operator != (const Attribute &) const
{
	return false;
}

std::string Attribute::typeName() const
{
	return BufferManager::getTypeName(BufferManager::files[item.type]->elems[index]);
}

Attribute Item::operator [] (std::size_t attrno)
{
	return Attribute(*this, attrno);
}

std::string Item::typeName() const
{
	return BufferManager::demangle(type);
}

}

}