#pragma once

#include <buffer/buffer_aux.h>
#include <debug/debug.hpp>

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
		// root = BufferManager::insertItem(1, { nullptr, nullptr, nullptr });
		// BufferManager::registerRoot(root);
		if (root[0] == nullptr) {
			debug::print::ln("left is nullptr");
		}
		// left = BufferManager::insertItem(1, );
	}
};

}

using __index::BPlusTree;

}