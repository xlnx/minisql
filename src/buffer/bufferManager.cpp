#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
#include <sstream>
#include <iostream>
#include <cstring>
#include <variant>
#include <utility>

#define SE_FNAME(suffix) (std::string("./SSBsb3ZlIGtvaXNoaS1jaGFu") + suffix + ".dat")

namespace minisql
{

namespace __buffer
{

std::vector<File*> BufferManager::files;
heap<Block*> BufferManager::cachedBlocks;
OffType BufferManager::offindex;
ItemIndex BufferManager::erased;

static BufferManager dummy;

struct BufferManagerInfo
{
	SizeType numBufferTypes;
	ItemIndex erased;
};

struct BufferTypeInfo
{
	OffType offset;
	SizeType numTypes;
	ItemIndex next;
	BufferType dataType;
	ItemIndex root;
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

BufferType BufferManager::registerBufferType(const std::vector<BufferElem> &elems)
{
	files.emplace_back(new File(BufferType(files.size()), elems, offindex, BufferType(files.size()), SQL_NAP));
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

BufferType BufferManager::registerBufferType(const std::vector<BufferElem> &elems, BufferType dataType)
{
	files.emplace_back(new File(BufferType(files.size()), elems, offindex, dataType, SQL_NULL));
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

void BufferManager::registerRoot(Item item)
{
	files[item.type]->root = item;
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
		info.dataType = (*iter)->dataType;
		info.root = (*iter)->root.index;
		cursor.write(reinterpret_cast<const char*>(&info), SizeOf(BufferTypeInfo));
	}
	writeHeader();
}

char *BufferManager::insert(BufferType scope, ItemIndex &index)
{
	auto &file = *files[scope];
	index = file.numDatas;
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
	block->isModified = true;
	return block->data + file.size * id;
}

char *BufferManager::read(BufferType scope, ItemIndex index)
{
	auto &file = *files[scope];
	auto blockid = index / file.blockCapacity;
	auto id = index % file.blockCapacity;
	auto block = file.blocks[blockid];
	ensureCached(*block);
	return block->data + file.size * id;
}

char *BufferManager::write(BufferType scope, ItemIndex index)
{
	auto &file = *files[scope];
	auto blockid = index / file.blockCapacity;
	auto id = index % file.blockCapacity;
	auto block = file.blocks[blockid];
	ensureCached(*block);
	block->isModified = true;
	return block->data + file.size * id;
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

void BufferManager::doWriteAttribute(File &file, char *dest, const AttributeType &val, SizeType i)
{
	switch ((file.elems[i] & 0xff0000) >> 16)
	{
		case 0x10: {		// SQL_INT
			memcpy(dest + file.attrOffset[i], &val, sizeof(int));
		} break;
		case 0x40: {		// SQL_FLOAT
			memcpy(dest + file.attrOffset[i], &val, sizeof(float));
		} break;
		case 0x20: {
			memcpy(dest + file.attrOffset[i], reinterpret_cast<const std::string*>(&val)->c_str(), 
				file.elems[i] & 0x00ffff);
		} break;
		case 0x80: {
			if (std::holds_alternative<void*>(val))
			{
				ItemIndex null = SQL_NULL;
				memcpy(dest + file.attrOffset[i], &null, sizeof(ItemIndex));
			}
			else
			{
				memcpy(dest + file.attrOffset[i], &reinterpret_cast<const Item*>(&val)->index,
					sizeof(ItemIndex));
			}
		} break;
	}
}

AttributeType BufferManager::doReadAttribute(File &file, char *dest, SizeType i)
{
	switch ((file.elems[i] & 0xff0000) >> 16)
	{
		case 0x10: {		// SQL_INT
			return *reinterpret_cast<int*>(dest + file.attrOffset[i]);
		} break;
		case 0x40: {		// SQL_FLOAT
			return *reinterpret_cast<float*>(dest + file.attrOffset[i]);
		} break;
		case 0x20: {
			auto beg = dest + file.attrOffset[i];
			return std::string(beg, beg + (file.elems[i] & 0x00ffff));
		} break;
		case 0x80: {
			switch (file.elems[i] & 0x0000ff)
			{
				case 0x01: {	// DATA
					return Item(file.dataType, *reinterpret_cast<ItemIndex*>(dest + file.attrOffset[i]));
				} break;
				case 0x02: {	// NODE
					return Item(file.type, *reinterpret_cast<ItemIndex*>(dest + file.attrOffset[i]));
				} break;
			}
		} break;
	}
	throw 0;
}

void BufferManager::doWriteItem(File &file, char *dest, const ItemValue &data)
{
	for (SizeType i = 0; i != file.elems.size(); ++i)
	{
		doWriteAttribute(file, dest, data[i], i);
	}
}

ItemValue BufferManager::doReadItem(File &file, char *dest)
{
	ItemValue val;
	for (SizeType i = 0; i != file.elems.size(); ++i)
	{
		val.emplace_back(doReadAttribute(file, dest, i));
	}
	return val;
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
		offindex = OffType(cursor.tellg());
		cursor.read(reinterpret_cast<char*>(&bi), SizeOf(BufferTypeInfo));
		if (bi.next == SQL_NAP)
		{
			cursor.seekg(bi.offset, std::ios::beg);
			elems.resize(bi.numTypes);
			cursor.read(reinterpret_cast<char*>(&elems[0]), SizeOf(BufferElem) * bi.numTypes);
			files.emplace_back(new File(i - 1, elems, bi.offset, bi.dataType, bi.root));
		}
		else
		{
			files.emplace_back(new File(i - 1, bi.next, bi.offset, bi.dataType, bi.root));
		}
	}
	std::vector<int> rels;
	for (auto file: files)
	{
		if (file->valid) 
		{
			rels.emplace_back(file->dataType);
		}
		else
		{
			rels.emplace_back(-1);
		}
	}
	for (auto i = 0u; i != rels.size(); ++i)
	{
		if (rels[i] != i && rels[i] != -1)
		{
			auto e = files[i]->root;
		} 
	}
	IndexManager::initialize(rels);
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