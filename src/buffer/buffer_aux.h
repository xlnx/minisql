#pragma once

// #include "buffer/io.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <debug/debug.hpp>

#define BLOCK_SIZE (1024 * 4)

#define SQL_INT 0x100000
#define SQL_CHAR(N) (0x200000 | N)
#define SQL_FLOAT 0x400000
#define SQL_POINTER 0x800000
#define SQL_NONE_TYPE 0x0

#define MAX_CACHE_BLOCK_COUNT (1024) // 4kb * 1024 = 4 MB

#define SQL_NULL (-1)
#define SQL_NAP (-2)

namespace minisql
{

namespace __buffer
{

using BufferType = unsigned short;
using BufferElem = unsigned;
using Pointer = int;
using SizeType = int;
using DataIndex = unsigned int;

template <typename ...Args>
using heap = std::vector<Args...>;

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
	File(const std::vector<BufferElem> &elems, Pointer offset, SizeType id);

	File(Pointer next, Pointer offset, SizeType id);

	std::fstream &cursor() { return fs; }

	void addBlock();

	void writeHeader();
public:
	SizeType size = 0;
	std::vector<BufferElem> elems;
	Pointer next = SQL_NAP;
	Pointer offset;

	std::vector<Block> blocks;
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
private:
	static void writeHeader();
	static void writeIndex();
public:
	BufferManager();
	~BufferManager();

	static BufferType registerBufferType(const std::vector<BufferElem> &elems);

	static void ensureCached(Block &block);

	// static const BufferElemInfo &getElemInfo(BufferType type) { return files[type]; }

	static DataIndex insert(BufferType scope, const char *data);

	static char *get(BufferType scope, DataIndex index);
};

// query -> block=2/bid=2 -> file -> 
}

using __buffer::BufferManager;
using __buffer::BufferType;
using __buffer::DataIndex;

}