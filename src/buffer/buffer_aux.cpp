#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
#include <sstream>
#include <iostream>
#include <cstring>

#define SE_FNAME(suffix) (std::string("./SSBsb3ZlIGtvaXNoaS1jaGFu") + suffix + ".dat")

namespace minisql
{

namespace __buffer
{

struct FileHeader
{
	SizeType numBlocks;
	SizeType numDatas;
	ItemIndex erased;
	ItemIndex root;
};

static void open(std::fstream &fs, BufferType id, ItemIndex root)
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
		h.root = root;
		fs.write(reinterpret_cast<const char*>(&h), SizeOf(FileHeader));
		fs.close();
		fs.open(SE_FNAME(os.str()), std::ios::binary | std::ios::in | std::ios::out);
	}
}

File::File(BufferType type, const std::vector<BufferElem> &elems, OffType offset, BufferType dataType):
	elems(elems), offset(offset), type(type), dataType(dataType), valid(true)
{
	for (auto e: elems)
	{
		attrOffset.emplace_back(size);
		switch ((e & 0xff0000) >> 16)
		{
			case 0x10: {	// SQL_INT
				size += SizeOf(int); 
			} break;
			case 0x40: {	// SQL_INT
				size += SizeOf(float); 
			} break;
			case 0x20: {	// SQL_CHAR(N)
				auto n = e & 0x00ffff;
				size += n;
			} break;
			case 0x80: {	// SQL_POINTER
				size += SizeOf(ItemIndex);
			} break;
		}
	}
	blockCapacity = BLOCK_SIZE / size;
	open(fs, type, dataType == type ? SQL_NAP : SQL_NULL);
	FileHeader h;
	fs.seekg(- SizeOf(FileHeader), std::ios::end);
	fs.read(reinterpret_cast<char*>(&h), SizeOf(FileHeader));
	erased = h.erased;
	numDatas = h.numDatas;
	root = Item(type, h.root);
	SizeType idx = 0;
	for (auto i = 0; i != h.numBlocks; ++i)
	{
		auto ncap = 0;
		blocks.emplace_back(new Block(*this, BLOCK_SIZE * i));
		while (idx < h.numDatas && ncap++ < blockCapacity)
		{
			// TODO: ensure no erased. no need to keep index = sub
			items.emplace_back(type, idx++);
		}
	}
}

File::File(BufferType type, ItemIndex next, OffType offset, BufferType dataType):
	next(next), offset(offset), type(type), dataType(dataType), valid(false)
{
	blockCapacity = BLOCK_SIZE / size;
	open(fs, type, dataType == type ? SQL_NAP : SQL_NULL);
	FileHeader h;
	fs.seekg(- SizeOf(FileHeader), std::ios::beg);
	fs.read(reinterpret_cast<char*>(&h), SizeOf(FileHeader));
	erased = h.erased;
	numDatas = h.numDatas;
	root = Item(type, h.root);
	for (auto i = 0; i != h.numBlocks; ++i)
	{
		blocks.emplace_back(new Block(*this, BLOCK_SIZE * i));
	}
}

File::~File()
{
	writeHeader();
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
	h.root = root.index;
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
Block::Block(File &f, OffType offset):
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

}

}