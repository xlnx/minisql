#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
#include <sstream>
#include <iostream>
#include <cstring>
#include <variant>
#include <utility>

namespace minisql
{

namespace __buffer
{

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
};

BufferType BufferManager::registerBufferType(const std::vector<BufferElem> &elems)
{
	BufferType id;
	if (erased != SQL_NULL)
	{
		id = erased;
		erased = files[id]->next;
		delete files[id];
		files[id] = new File(id, elems, offindex, id, true);
	}
	else
	{
		id = files.size();
		files.emplace_back(new File(id, elems, offindex, id));
	}
	auto file = files[id];
	offindex += SizeOf(BufferElem) * elems.size();
	auto &cursor = icursor;
	cursor.seekp(file->offset, std::ios::beg);
	cursor.write(reinterpret_cast<const char*>(&file->elems[0]), SizeOf(BufferElem) * file->elems.size());
	writeIndex();
	return id;
}

BufferType BufferManager::registerBufferType(const std::vector<BufferElem> &elems, BufferType dataType)
{
	BufferType id;
	if (erased != SQL_NULL)
	{
		id = erased;
		erased = files[id]->next;
		delete files[id];
		files[id] = new File(id, elems, offindex, dataType, true);
	}
	else
	{
		id = files.size();
		files.emplace_back(new File(id, elems, offindex, dataType));
	}
	auto file = files[id];
	offindex += SizeOf(BufferElem) * elems.size();
	auto &cursor = icursor;
	cursor.seekp(file->offset, std::ios::beg);
	cursor.write(reinterpret_cast<const char*>(&file->elems[0]), SizeOf(BufferElem) * file->elems.size());
	writeIndex();
	return id;
}

void BufferManager::removeBufferType(BufferType type)
{
	files[type]->invalidate();
	files[type]->next = erased;
	erased = type;
	writeIndex();
}

void BufferManager::registerRoot(Item item)
{
	files[item.type]->root = item;
}

void BufferManager::writeHeader()
{
	BufferManagerInfo info;
	info.numBufferTypes = files.size();
	info.erased = erased;
	auto &cursor = icursor;
	cursor.seekp(offindex + files.size() * SizeOf(BufferTypeInfo), std::ios::beg);
	cursor.write(reinterpret_cast<const char*>(&info), SizeOf(BufferManagerInfo));
}

void BufferManager::writeIndex()
{
	auto &cursor = icursor;
	cursor.seekp(offindex, std::ios::beg);
	BufferTypeInfo info;
	for (auto iter = files.rbegin(); iter != files.rend(); ++iter)
	{
		info.next = (*iter)->next;
		info.numTypes = (*iter)->elems.size();
		info.offset = (*iter)->offset;
		info.dataType = (*iter)->dataType;
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
		memcpy(&file.erased, block->data + file.size * (id + 1) - SizeOf(ItemIndex), SizeOf(ItemIndex));
	}
	else
	{
		if (id == 0) file.addBlock();
		block = file.blocks.back();
		ensureCached(*block);
		file.numDatas++;
	}
	auto ptr = block->data + file.size * id;
	RefCount n = -3;
	memcpy(ptr + file.attrOffset.back(), reinterpret_cast<const char*>(&n), sizeof(RefCount));
	block->isModified = true;
	return ptr;
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
		case 0x40: {
			os << "float";
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

void BufferManager::ensureCached(Block &block)
{
	if (!block.data)
	{
		if (cachedBlocks.size() > MAX_CACHE_BLOCK_COUNT)
		{		// delete cache
			cachedBlocks.front()->release();
			cachedBlocks.pop_front();
		}
		block.cache();
		cachedBlocks.push_back(&block);
	}
}

AttributeValue BufferManager::doReadAttribute(File &file, char *dest, SizeType i)
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
			auto s = std::string(beg, beg + (file.elems[i] & 0x00ffff));
			return s.substr(0, s.find('\0'));
		} break;
		case 0x80: {
			auto idx = *reinterpret_cast<ItemIndex*>(dest + file.attrOffset[i]);
			if (idx == SQL_NULL) {
				return NullType();
			}
			else switch (file.elems[i] & 0x0000ff)
			{
				case 0x01: {	// DATA
					return Item(file.dataType, idx);
				} break;
				case 0x02: {	// NODE
					return Item(file.type, idx);
				} break;
			}
		} break;
	}
	throw 0;
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

void BufferManager::doWriteAttribute(File &file, char *dest, const AttributeValue &val, SizeType i)
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
			if (std::holds_alternative<NullType>(val))
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

void BufferManager::removeRef(Item item)
{
	auto &file = *files[item.type];
	auto ptr = write(item.type, item.index);
	auto &ref = *reinterpret_cast<RefCount*>(ptr + file.attrOffset.back());
	if (++ref >= -3)
	{
		ref = file.erased; file.erased = item.index;
	}
}

void BufferManager::addRef(Item item)
{
	auto &file = *files[item.type];
	auto ptr = write(item.type, item.index);
	--*reinterpret_cast<RefCount*>(ptr + file.attrOffset.back());
}

BufferManager::BufferManager()
{
	std::ios::sync_with_stdio(false);
	auto &cursor = icursor;
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
			files.emplace_back(new File(i - 1, elems, bi.offset, bi.dataType));
		}
		else
		{
			files.emplace_back(new File(i - 1, bi.next, bi.offset, bi.dataType));
		}
	}
	std::vector<std::pair<int, AttributeValue>> rels;
	for (auto file: files)
	{
		if (file->valid) 
		{
			if (file->dataType != file->type)
			{
				rels.emplace_back(file->dataType, file->root);
			}
			else
			{
				rels.emplace_back(file->dataType, NullType());
			}
		}
		else
		{
			rels.emplace_back(-1, NullType());
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