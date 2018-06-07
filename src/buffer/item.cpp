#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
#include <sstream>
#include <iostream>
#include <cstring>

namespace minisql
{

namespace __buffer
{

Attribute::Attribute(Item item, SizeType index):
	item(item), index(index), value(BufferManager::readAttribute(*this))
{
}

std::string Attribute::typeName() const
{
	return BufferManager::getTypeName(BufferManager::files[item.type]->elems[index]);
}

std::string Item::typeName() const
{
	return BufferManager::demangle(type);
}

}

}