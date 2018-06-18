#pragma once

#include <buffer/buffer_def.h>
#include <iostream>
#include <utility>
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace minisql
{

namespace __buffer
{

class Item;

struct NullType 
{
	bool operator < (const NullType &) const { return true; }
	bool operator <= (const NullType &) const { return true; }
	bool operator > (const NullType &) const { return true; }
	bool operator >= (const NullType &) const { return true; }
	bool operator == (const NullType &) const { return true; }
	bool operator != (const NullType &) const { return true; }
};

using AttributeValue = std::variant<std::string, int, float, Item, NullType>;
using ItemValue = std::vector<AttributeValue>;

class Attribute;

class Item
{
	friend class File;
	friend class Attribute;
	friend class BufferManager;
private:
	ItemIndex index = SQL_NAP;
	BufferType type = 0;
public:
	const bool operator < (const Item &other) const
		{ return index < other.index; }
	const bool operator <= (const Item &other) const
		{ return index <= other.index; }
	const bool operator > (const Item &other) const
		{ return index > other.index; }
	const bool operator >= (const Item &other) const
		{ return index >= other.index; }
	const bool operator == (const Item &other) const
		{ return index == other.index; }
	const bool operator != (const Item &other) const
		{ return index != other.index; }

	Item() = default;
	Item(BufferType type, ItemIndex index): index(index), type(type) {}

	const Attribute operator [] (std::size_t attrno) const;
	const Attribute attr (std::size_t attrno) const;

	const std::string typeName() const;

	friend std::ostream &operator << (std::ostream &os, const Item &item);
	friend std::ostream &operator << (std::ostream &os, const Attribute &attr);
};

class Attribute
{
	friend class Item;
	friend class BufferManager;
private:
	Item item;
	SizeType index;
	AttributeValue value;

	Attribute(Item item, SizeType index);
public:
	Attribute() = default;
	
	const bool operator < (const Attribute &other) const
		{ return value < other.value; }
	const bool operator <= (const Attribute &other) const
		{ return value <= other.value; }
	const bool operator > (const Attribute &other) const
		{ return value > other.value; }
	const bool operator >= (const Attribute &other) const
		{ return value >= other.value; }
	const bool operator == (const Attribute &other) const
		{ return value == other.value; }
	const bool operator != (const Attribute &other) const
		{ return value != other.value; }

	const bool operator == (nullptr_t) const
		{ return std::holds_alternative<NullType>(value); }
	const bool operator != (nullptr_t) const 
		{ return !std::holds_alternative<NullType>(value); }

	const Item operator * () const
		{ return std::get<Item>(value); }
	const Item operator -> () const 
		{ return this->operator*(); }

	const std::string typeName() const;

	operator const AttributeValue & () const
		{ return value; }
	
	const AttributeValue &val() const
		{ return value; }

	friend std::ostream &operator << (std::ostream &os, const Attribute &attr);
};

inline const Attribute Item::operator [] (std::size_t attrno) const
{
	return Attribute(*this, attrno);
}

class File;

class Block final
{
	File &ffile;
public:
	Block(File &f, OffType offset);

	void cache();

	void dump();

	void release();

	File &file() { return ffile; }
public:
	char *data = nullptr;
	OffType offset;
	bool isModified = false;
	bool isDeleted = false;
};


class File final
{
	friend class BufferManager;

public:
	File(BufferType type, const ItemType &elems, OffType offset, BufferType dataType, bool force = false);

	File(BufferType type, ItemIndex next, OffType offset, BufferType dataType);

	~File();

	void addBlock();

	void writeHeader();

	void invalidate();
public:
	std::fstream cursor;

	SizeType size = 0;
	ItemType elems;
	std::vector<SizeType> attrOffset;
	// std::vector<void> attrDecoder;
	ItemIndex next = SQL_NAP;
	OffType offset;

	std::vector<Block*> blocks;
	SizeType numDatas;
	SizeType blockCapacity;
	ItemIndex erased;

	BufferType type;
	BufferType dataType;
	bool valid;
	Item root;
};

class BufferManager final
{
	static std::vector<File*> files;
	static heap<Block*> cachedBlocks;
	static OffType offindex;
	static ItemIndex erased;
	static std::fstream icursor;

	friend class Attribute;
	friend class Item;
private:
	static void writeHeader();
	static void writeIndex();

	static void ensureCached(Block &block);
	static std::string getTypeName(BufferElem);

	static char *insert(BufferType scope, ItemIndex &index);
	static char *read(BufferType scope, ItemIndex index);
	static char *write(BufferType scope, ItemIndex index);

	static void doWriteItem(File &file, char *dest, const ItemValue &data);
	static ItemValue doReadItem(File &file, char *dest);

	static void doWriteAttribute(File &file, char *dest, const AttributeValue &val, SizeType index);
	static AttributeValue doReadAttribute(File &file, char *dest, SizeType i);
public:
	BufferManager();
	~BufferManager();

	static std::string demangle(BufferType type);

	static BufferType registerBufferType(const ItemType &elems);
	static BufferType registerBufferType(const ItemType &elems, BufferType dataType);
	static void registerRoot(Item item);

	static void removeBufferType(BufferType type);

	static const ItemType &getItemType(BufferType type)
	{
		return files[type]->elems;
	}

	static Item insertItem(BufferType type, const ItemValue &data)
	{
		ItemIndex index;
		doWriteItem(*files[type], insert(type, index), data);
		return Item(type, index);
	}
	static void writeItem(Item item, const ItemValue &data)
	{
		doWriteItem(*files[item.type], write(item.type, item.index), data);
	}
	static ItemValue readItem(Item item)
	{
		return doReadItem(*files[item.type], read(item.type, item.index));
	}

	static void writeAttribute(const Attribute &attr, const AttributeValue &val)
	{
		doWriteAttribute(*files[attr.item.type], write(attr.item.type, attr.item.index), val, attr.index);
	}
	static AttributeValue readAttribute(const Attribute &attr)
	{
		return doReadAttribute(*files[attr.item.type], read(attr.item.type, attr.item.index), attr.index);
	}

	// static void eraseItem(Item item);
	static void removeRef(Item item);
	static void addRef(Item item);
};

inline Attribute::Attribute(Item item, SizeType index):
	item(item), index(index), value(BufferManager::readAttribute(*this))
{
}

// query -> block=2/bid=2 -> file -> 
}

using __buffer::BufferManager;
using __buffer::BufferType;
using __buffer::ItemIndex;
using __buffer::Item;
using __buffer::Attribute;
using __buffer::AttributeValue;
using __buffer::ItemValue;
using __buffer::NullType;

}