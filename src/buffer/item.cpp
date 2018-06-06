#include <buffer/item.h>
#include <buffer/buffer_aux.h>

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
	return BufferManager::getTypeName(type);
}

// Item
Attribute Item::operator [] (std::size_t attrno)
{
	auto &file = *BufferManager::files[index];
	// file.elems[attrno];
	return Attribute(
		file.elems[attrno], 
		BufferManager::read(type, index) + file.attrOffset[attrno]
	);
}

std::string Item::typeName() const
{
	return BufferManager::demangle(type);
}

}

}