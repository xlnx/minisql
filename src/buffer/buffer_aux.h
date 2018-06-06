#pragma once

#include <buffer/buffer_def.h>
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

using AttributeType = std::variant<std::string, int, float, Item, void*>;
using ItemValue = std::vector<AttributeType>;

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
	Item() = default;
	Item(BufferType type, ItemIndex index): index(index), type(type) {}

	Attribute operator [] (std::size_t attrno);

	std::string typeName() const;
};

class Attribute
{
	friend class Item;
	friend class BufferManager;
private:
	Item item;
	SizeType index;

	Attribute(Item item, SizeType index): item(item), index(index) {}
public:
	bool operator < (const Attribute &) const;
	bool operator <= (const Attribute &) const;
	bool operator > (const Attribute &) const;
	bool operator >= (const Attribute &) const;
	bool operator == (const Attribute &) const;
	bool operator != (const Attribute &) const;

	std::string typeName() const;
};

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
};


class File final
{
	friend class BufferManager;
	std::fstream fs;

public:
	File(BufferType type, const ItemType &elems, OffType offset, 
		BufferType dataType, ItemIndex root);

	File(BufferType type, ItemIndex next, OffType offset, 
		BufferType dataType, ItemIndex root);

	~File();

	std::fstream &cursor() { return fs; }

	void addBlock();

	void writeHeader();
public:
	SizeType size = 0;
	ItemType elems;
	std::vector<SizeType> attrOffset;
	// std::vector<void> attrDecoder;
	ItemIndex next = SQL_NAP;
	OffType offset;

	std::vector<Block*> blocks;
	std::vector<Item> items;
	SizeType numDatas;
	SizeType blockCapacity;
	SizeType erased;

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
	static SizeType erased;

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

	static void doWriteAttribute(File &file, char *dest, const AttributeType &val, SizeType index);
	static AttributeType doReadAttribute(File &file, char *dest, SizeType index);
public:
	BufferManager();
	~BufferManager();

	static std::string demangle(BufferType type);

	static BufferType registerBufferType(const ItemType &elems);
	static BufferType registerBufferType(const ItemType &elems, BufferType dataType);
	static void registerRoot(Item item);

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

	static void writeAttribute(Attribute attr, const AttributeType &val)
	{
		doWriteAttribute(*files[attr.item.type], write(attr.item.type, attr.item.index), val, attr.index);
	}
	static AttributeType readAttribute(Attribute attr)
	{
		return doReadAttribute(*files[attr.item.type], read(attr.item.type, attr.item.index), attr.index);
	}
};

// query -> block=2/bid=2 -> file -> 
}

using __buffer::BufferManager;
using __buffer::BufferType;
using __buffer::ItemIndex;
using __buffer::Item;
using __buffer::Attribute;
using __buffer::AttributeType;
using __buffer::ItemValue;

}