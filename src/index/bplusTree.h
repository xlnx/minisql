#pragma once

#include <buffer/buffer_aux.h>

namespace minisql
{

namespace __index
{

class BPlusTree final
{
	Item root;
public:
	BPlusTree() = default;

	void init()
	{
		root = BufferManager::insertItem(1, { nullptr, nullptr, nullptr });
		BufferManager::registerRoot(root);
		// left = BufferManager::insertItem(1, );
	}
};

}

using __index::BPlusTree;

}