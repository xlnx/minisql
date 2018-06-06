#include "buffer/buffer_aux.h"
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
	return BufferManager::getTypeName(type);
}

// Item
Attribute Item::operator [] (std::size_t attrno)
{
	auto &file = *BufferManager::files[index];
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

namespace __buffer
{

std::vector<File*> BufferManager::files;
heap<Block*> BufferManager::cachedBlocks;
Pointer BufferManager::offindex;
Pointer BufferManager::erased;

static BufferManager dummy;

#define SE_FNAME(suffix) (std::string("./SSBsb3ZlIGtvaXNoaS1jaGFu") + suffix + ".dat")

struct BufferManagerInfo
{
	SizeType numBufferTypes;
	Pointer erased;
};

struct BufferTypeInfo
{
	Pointer offset;
	SizeType numTypes;
	Pointer next;
};

static std::fstream &icursor()
{
	static std::fstream cur = []() -> std::fstream {
		std::fstream cur;
		cur.open(SE_FNAME(""), std::ios::binary | std::ios::in | std::ios::out);
		if (!cur) {
			cur.open(SE_FNAME(""), std::ios::binary | std::ios::trunc | std::ios::out);
			cur.seekp(0, std::ios::beg);
			BufferManagerInfo info;
			info.numBufferTypes = 0;
			info.erased = SQL_NULL;
			cur.write(reinterpret_cast<const char*>(&info), SizeOf(BufferManagerInfo));
			cur.close();
			cur.open(SE_FNAME(""), std::ios::binary | std::ios::in | std::ios::out);
		}
		return cur;
	} ();
	return cur;
}

// File
struct FileHeader
{
	SizeType numBlocks;
	SizeType numDatas;
	Pointer erased;
};

static void open(std::fstream &fs, SizeType id)
{
	std::ostringstream os;
	os << std::dec << id;
	fs.open(SE_FNAME(os.str()), std::ios::binary | std::ios::in | std::ios::out);
	if (!fs)
	{
		fs.open(SE_FNAME(os.str()), std::ios::binary | std::ios::trunc | std::ios::out);
		FileHeader h;
		h.numBlocks = 0;
		h.numDatas = 0;
		h.erased = SQL_NULL;
		fs.write(reinterpret_cast<const char*>(&h), SizeOf(FileHeader));
		fs.close();
		fs.open(SE_FNAME(os.str()), std::ios::binary | std::ios::in | std::ios::out);
	}
}

File::File(BufferType type, const std::vector<BufferElem> &elems, Pointer offset):
	elems(elems), offset(offset), type(type)
{
	for (auto e: elems)
	{
		attrOffset.emplace_back(size);
		switch ((e & 0xff0000) >> 16)
		{
			case 0x10: {	// SQL_INT
				size += SizeOf(int); 
			} break;
			case 0x20: {	// SQL_CHAR(N)
				auto n = e & 0x00ffff;
				size += n;
			} break;
			case 0x80: {	// SQL_POINTER
				size += SizeOf(int);
			} break;
		}
	}
	blockCapacity = BLOCK_SIZE / size;
	open(fs, type);
	FileHeader h;
	fs.seekg(- SizeOf(FileHeader), std::ios::end);
	fs.read(reinterpret_cast<char*>(&h), SizeOf(FileHeader));
	erased = h.erased;
	numDatas = h.numDatas;
	SizeType idx = 0;
	for (auto i = 0; i != h.numBlocks; ++i)
	{
		auto ncap = 0;
		blocks.emplace_back(new Block(*this, BLOCK_SIZE * i));
		while (idx < h.numDatas && ncap++ < blockCapacity)
		{
			items.emplace_back(type, idx++);
		}
	}
}

File::File(BufferType type, Pointer next, Pointer offset):
	next(next), offset(offset), type(type)
{
	blockCapacity = BLOCK_SIZE / size;
	open(fs, type);
	FileHeader h;
	fs.seekg(- SizeOf(FileHeader), std::ios::beg);
	fs.read(reinterpret_cast<char*>(&h), SizeOf(FileHeader));
	erased = h.erased;
	numDatas = h.numDatas;
	for (auto i = 0; i != h.numBlocks; ++i)
	{
		blocks.emplace_back(new Block(*this, BLOCK_SIZE * i));
	}
}

File::~File()
{
	for (auto b: blocks)
	{
		delete b;
	}
}

void File::writeHeader()
{
	FileHeader h;
	h.erased = erased;
	h.numDatas = numDatas;
	h.numBlocks = blocks.size();
	fs.seekp(BLOCK_SIZE * h.numBlocks, std::ios::beg);
	// debug::print::state(fs);
	fs.write(reinterpret_cast<const char*>(&h), SizeOf(FileHeader));
	// debug::hl([&]() {
	// 	debug::print::state(fs);
	// });
}

void File::addBlock()
{
	blocks.emplace_back(new Block(*this, BLOCK_SIZE * blocks.size()));
	writeHeader();
}

// Block
Block::Block(File &f, Pointer offset):
	ffile(f), offset(offset)
{
}

void Block::cache()
{
	// debug::print::state(ffile.cursor());
	ffile.cursor().seekg(offset, std::ios::beg);
	// BufferManager::getElemInfo(ffile.elemType()).size() * 
	data = new char [BLOCK_SIZE]; ffile.cursor().read(data, BLOCK_SIZE);
}

void Block::dump()
{
	ffile.cursor().seekp(offset, std::ios::beg);
	ffile.cursor().write(data, BLOCK_SIZE); 
}

void Block::release()
{
	if (isModified) { dump(); }
	delete [] data; data = nullptr;
}

// BufferManager
BufferType BufferManager::registerBufferType(const std::vector<BufferElem> &elems)
{
	files.emplace_back(new File(BufferType(files.size()), elems, offindex));
	offindex += SizeOf(BufferElem) * elems.size();
	auto &cursor = icursor();
	auto &file = *files.back();
	cursor.seekp(file.offset, std::ios::beg);
	// debug::print::ln(file.offset, cursor.tellg(), cursor.tellp());
	// debug::print::mem(*reinterpret_cast<BufferElem(*)[4]>(&file.elems[0]));
	cursor.write(reinterpret_cast<const char*>(&file.elems[0]), SizeOf(BufferElem) * file.elems.size());
	writeIndex();
	return BufferType(files.size() - 1);
}

void BufferManager::ensureCached(Block &block)
{
	if (!block.data)
	{
		if (cachedBlocks.size() > MAX_CACHE_BLOCK_COUNT)
		{		// delete cache
			cachedBlocks.back()->release();
			cachedBlocks.pop_back();
		}
		block.cache();
		cachedBlocks.push_back(&block);
	}
}

void BufferManager::writeHeader()
{
	BufferManagerInfo info;
	info.numBufferTypes = files.size();
	info.erased = erased;
	auto &cursor = icursor();
	cursor.seekp(offindex + files.size() * SizeOf(BufferTypeInfo), std::ios::beg);
	cursor.write(reinterpret_cast<const char*>(&info), SizeOf(BufferManagerInfo));
}

void BufferManager::writeIndex()
{
	auto &cursor = icursor();
	cursor.seekp(offindex, std::ios::beg);
	BufferTypeInfo info;
	for (auto iter = files.rbegin(); iter != files.rend(); ++iter)
	{
		info.next = (*iter)->next;
		info.numTypes = (*iter)->elems.size();
		info.offset = (*iter)->offset;
		cursor.write(reinterpret_cast<const char*>(&info), SizeOf(BufferTypeInfo));
	}
	writeHeader();
}

DataIndex BufferManager::insert(BufferType scope, const char *data)
{
	auto &file = *files[scope];
	auto index = file.numDatas;
	auto id = index % file.blockCapacity;
	Block *block;
	if (file.erased != SQL_NULL)
	{
		index = file.erased;
		block = file.blocks[index / file.blockCapacity];
		id = file.erased % file.blockCapacity;
		ensureCached(*block);
		memcpy(&file.erased, block->data + file.size * id, SizeOf(SizeType));
	}
	else
	{
		if (id == 0) file.addBlock();
		block = file.blocks.back();
		ensureCached(*block);
		file.numDatas++;
	}
	memcpy(block->data + file.size * id, data, file.size);
	// debug::print::ln(block);
	block->isModified = true;
	file.writeHeader();
	// debug::print::state(file.cursor());
	return index;
}

char *BufferManager::read(BufferType scope, DataIndex index)
{
	auto &file = *files[scope];
	auto blockid = index / file.blockCapacity;
	auto id = index % file.blockCapacity;
	auto block = file.blocks[blockid];
	ensureCached(*block);
	return block->data + file.size * id;
}

void BufferManager::write(BufferType scope, DataIndex index, const char *data)
{
	auto &file = *files[scope];
	auto blockid = index / file.blockCapacity;
	auto id = index % file.blockCapacity;
	auto block = file.blocks[blockid];
	ensureCached(*block);
	memcpy(block->data + file.size * id, data, file.size);
	block->isModified = true;
}

std::string BufferManager::getTypeName(BufferElem type)
{
	std::ostringstream os;
	switch ((type & 0xff0000) >> 16)
	{
		case 0x10: {	// SQL_INT
			os << "int";
		} break;
		case 0x20: {	// SQL_CHAR(N)
			os << "char(" << std::dec << (type & 0x00ffff) << ")";
		} break;
		case 0x80: {	// SQL_POINTER
			os << "object";
		} break;
	}
	return os.str();
}

std::string BufferManager::demangle(BufferType type)
{
	auto &elems = files[type]->elems;
	std::string str = getTypeName(elems[0]);
	for (auto i = 1u; i < elems.size(); ++i) 
	{
		str += "," + getTypeName(elems[i]);
	}
	return str;
}

// return Item(type, index);

Item BufferManager::insertItem(BufferType type, const char *data)
{
	auto index = insert(type, data);
	return Item(type, index);
}

void BufferManager::writeItem(Item item, const char * data)
{
	write(item.type, item.index, data);
}

BufferManager::BufferManager()
{
	auto &cursor = icursor();
	BufferManagerInfo info;
	cursor.seekg(- SizeOf(BufferManagerInfo), std::ios::end);
	cursor.read(reinterpret_cast<char*>(&info), SizeOf(BufferManagerInfo));
	erased = info.erased;
	BufferTypeInfo bi;
	std::vector<BufferElem> elems;
	for (int i = 1; i <= info.numBufferTypes; ++i)
	{
		cursor.seekg(- i * SizeOf(BufferTypeInfo) - SizeOf(BufferManagerInfo), std::ios::end);
		offindex = Pointer(cursor.tellg());
		cursor.read(reinterpret_cast<char*>(&bi), SizeOf(BufferTypeInfo));
		if (bi.next == SQL_NAP)
		{
			cursor.seekg(bi.offset, std::ios::beg);
			elems.resize(bi.numTypes);
			cursor.read(reinterpret_cast<char*>(&elems[0]), SizeOf(BufferElem) * bi.numTypes);
			files.emplace_back(new File(i - 1, elems, bi.offset));
		}
		else
		{
			files.emplace_back(new File(i - 1, bi.next, bi.offset));
		}
	}
}

BufferManager::~BufferManager()
{
	for (auto block: cachedBlocks)
	{
		// debug::print::ln(block);
		block->release();
	}
	for (auto f: files)
	{
		delete f;
	}
}

}

}