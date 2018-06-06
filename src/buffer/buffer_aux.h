#pragma once

#include <buffer/buffer_def.h>
// #include <buffer/item.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace minisql
{

namespace __buffer
{

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
	File(const ItemType &elems, Pointer offset, SizeType id);

	File(Pointer next, Pointer offset, SizeType id);

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
	SizeType numDatas;
	SizeType blockCapacity;
	SizeType erased;
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
public:
	BufferManager();
	~BufferManager();

	static BufferType registerBufferType(const ItemType &elems);

	// static const BufferElemInfo &getElemInfo(BufferType type) { return files[type]; }

	static DataIndex insert(BufferType scope, const char *data);

	static char *read(BufferType scope, DataIndex index);

	static void write(BufferType scope, DataIndex index, const char *data);

	static std::string demangle(BufferType type);
};

// query -> block=2/bid=2 -> file -> 
}

using __buffer::BufferManager;
using __buffer::BufferType;
using __buffer::DataIndex;

}