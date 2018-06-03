#include "buffer/buffer_aux.h"
#include <sstream>
#include <iostream>

namespace minisql
{

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
			cur.write(reinterpret_cast<const char*>(&info), sizeof(BufferManagerInfo));
			cur.close();
			cur.open(SE_FNAME(""), std::ios::binary | std::ios::in | std::ios::out);
		}
		return cur;
	} ();
	return cur;
}

volatile int BufferManager::onStartup = [&]()
{
	auto &cursor = icursor();
	BufferManagerInfo info;
	cursor.seekg(- sizeof(BufferManagerInfo), std::ios::end);
	cursor.read(reinterpret_cast<char*>(&info), sizeof(BufferManagerInfo));
	erased = info.erased;
	BufferTypeInfo bi;
	std::vector<BufferElem> elems;
	for (int i = 1; i <= info.numBufferTypes; ++i)
	{
		cursor.seekg(- i * sizeof(BufferTypeInfo) - sizeof(BufferManagerInfo), std::ios::end);
		offindex = cursor.tellg();
		cursor.read(reinterpret_cast<char*>(&bi), sizeof(BufferTypeInfo));
		if (bi.next == SQL_NAP)
		{
			cursor.seekg(bi.offset, std::ios::beg);
			elems.resize(bi.numTypes);
			cursor.read(reinterpret_cast<char*>(&elems[0]), sizeof(BufferElem) * bi.numTypes);
			files.emplace_back(new File(elems, bi.offset, i - 1));
		}
		else
		{
			files.emplace_back(new File(bi.next, bi.offset, i - 1));
		}
	}
	return 0;
} ();

// File
struct FileHeader
{
	SizeType numBlocks;
	SizeType numDatas;
	Pointer erased;
};

static open(std::fstream &fs, SizeType id)
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
		fs.write(reinterpret_cast<const char*>(&h), sizeof(FileHeader));
		fs.close();
		fs.open(SE_FNAME(os.str()), std::ios::binary | std::ios::in | std::ios::out);
	}
}

File::File(const std::vector<BufferElem> &elems, Pointer offset, SizeType id):
	elems(elems), offset(offset)
{
	for (auto e: elems)
	{
		switch ((e & 0xff0000) >> 16)
		{
			case 0x10: {	// SQL_INT
				size += sizeof(int); 
			} break;
			case 0x20: {	// SQL_CHAR(N)
				auto n = e & 0x00ffff;
				size += n;
			} break;
			case 0x80: {	// SQL_POINTER
				size += sizeof(int);
			} break;
		}
	}
	open(fs, id);
	FileHeader h;
	fs.seekg(-sizeof(FileHeader), std::ios::end);
	fs.read(reinterpret_cast<char*>(&h), sizeof(FileHeader));
	erased = h.erased;
	numDatas = h.numDatas;
	for (auto i = 0; i != h.numBlocks; ++i)
	{
		blocks.emplace_back(*this, BLOCK_SIZE * i);
	}
	blockCapacity = BLOCK_SIZE / size;
}

File::File(Pointer next, Pointer offset, SizeType id):
	next(next), offset(offset)
{
	open(fs, id);
	FileHeader h;
	fs.seekg(-sizeof(FileHeader), std::ios::beg);
	fs.read(reinterpret_cast<char*>(&h), sizeof(FileHeader));
	erased = h.erased;
	numDatas = h.numDatas;
	for (auto i = 0; i != h.numBlocks; ++i)
	{
		blocks.emplace_back(*this, BLOCK_SIZE * i);
	}
	blockCapacity = BLOCK_SIZE / size;
}

void File::writeHeader()
{
	FileHeader h;
	h.erased = erased;
	h.numDatas = numDatas;
	h.numBlocks = blocks.size();
	fs.seekp(BLOCK_SIZE * h.numBlocks, std::ios::beg);
	debug::print::state(fs);
	fs.write(reinterpret_cast<const char*>(&h), sizeof(FileHeader));
	debug::hl([&]() {
		debug::print::state(fs);
	});
}

void File::addBlock()
{
	blocks.emplace_back(*this, BLOCK_SIZE * blocks.size());
	writeHeader();
}

// Block
Block::Block(File &f, Pointer offset):
	ffile(f), offset(offset)
{
}

void Block::cache()
{
	debug::print::state(ffile.cursor());
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
	files.emplace_back(new File(elems, offindex, files.size()));
	offindex += sizeof(BufferElem) * elems.size();
	auto &cursor = icursor();
	auto &file = *files.back();
	cursor.seekp(file.offset, std::ios::beg);
	// debug::print::ln(file.offset, cursor.tellg(), cursor.tellp());
	// debug::print::mem(*reinterpret_cast<BufferElem(*)[4]>(&file.elems[0]));
	cursor.write(reinterpret_cast<const char*>(&file.elems[0]), sizeof(BufferElem) * file.elems.size());
	writeIndex();
	return files.size() - 1;
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
	cursor.seekp(offindex + files.size() * sizeof(BufferTypeInfo), std::ios::beg);
	cursor.write(reinterpret_cast<const char*>(&info), sizeof(BufferManagerInfo));
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
		cursor.write(reinterpret_cast<const char*>(&info), sizeof(BufferTypeInfo));
	}
	writeHeader();
}

DataIndex BufferManager::insert(BufferType scope, const char *data)
{
	auto &file = *files[scope];
	auto id = file.numDatas % file.blockCapacity;
	Block *block;
	if (file.erased != SQL_NULL)
	{
		block = &file.blocks[file.erased / file.blockCapacity];
		id = file.erased % file.blockCapacity;
		ensureCached(*block);
		memcpy(&file.erased, block->data + file.size * id, sizeof(SizeType));
	}
	else
	{
		id = file.numDatas % file.blockCapacity;
		if (id == 0)
		{
			file.addBlock();
		}
		block = &file.blocks.back();
		ensureCached(*block);
	}
	memcpy(block->data + file.size * id, data, file.size);
	// debug::print::ln(block);
	block->isModified = true;
	file.numDatas++;
	file.writeHeader();
	debug::print::state(file.cursor());
}

char *BufferManager::get(BufferType scope, DataIndex index)
{
	auto &file = *files[scope];
	auto blockid = index / file.blockCapacity;
	auto id = index % file.blockCapacity;
	auto &block = file.blocks[blockid];
	ensureCached(block);
	return block.data + file.size * id;
}

BufferManager::~BufferManager()
{
	for (auto block: cachedBlocks)
	{
		debug::print::ln(block);
		block->release();
	}
	for (auto f: files)
	{
		delete f;
	}
}

}

}