#pragma once

#include <buffer/buffer_def.h>
#include <index/bplusTree.h>
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
	friend class BufferManager;
private:
	DataIndex index;
	BufferType type;
public:
	Item(BufferType type, DataIndex index): index(index), type(type) {}

	Attribute operator [] (std::size_t attrno);

	std::string typeName() const;
};

class File;

class Block final
{
	File &ffile;
public:
	Block(File &f, Pointer offset);

	void cache();

	void dump();

	void release();

	File &file() { return ffile; }
public:
	char *data = nullptr;
	Pointer offset;
	bool isModified = false;
};

class File final
{
	std::fstream fs;

public:
	File(BufferType type, const ItemType &elems, Pointer offset);

	File(BufferType type, Pointer next, Pointer offset);

	~File();

	std::fstream &cursor() { return fs; }

	void addBlock();

	void writeHeader();
public:
	SizeType size = 0;
	ItemType elems;
	std::vector<SizeType> attrOffset;
	// std::vector<void> attrDecoder;
	Pointer next = SQL_NAP;
	Pointer offset;

	std::vector<Block*> blocks;
	std::vector<Item> items;
	SizeType numDatas;
	SizeType blockCapacity;
	SizeType erased;

	BufferType type;
};

class BufferManager final
{
	static std::vector<File*> files;
	static heap<Block*> cachedBlocks;
	static Pointer offindex;
	static SizeType erased;

	friend class Attribute;
	friend class Item;
private:
	static void writeHeader();
	static void writeIndex();

	static void ensureCached(Block &block);
	static std::string getTypeName(BufferElem);

	static DataIndex insert(BufferType scope, const char *data);
	static char *read(BufferType scope, DataIndex index);
	static void write(BufferType scope, DataIndex index, const char *data);
public:
	BufferManager();
	~BufferManager();

	static BufferType registerBufferType(const ItemType &elems);

	static std::string demangle(BufferType type);

	static Item insertItem(BufferType type, const char *data);

	static void writeItem(Item item, const char *data);
};

// query -> block=2/bid=2 -> file -> 
}

using __buffer::BufferManager;
using __buffer::BufferType;
using __buffer::DataIndex;
using __buffer::Item;
using __buffer::Attribute;

}