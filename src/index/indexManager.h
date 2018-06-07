#pragma once

#include <buffer/buffer_aux.h>
#include <map>
#include<vector>
#include <utility>

namespace minisql
{

namespace __index
{

using std::map;
using std::vector;
using std::pair;

class IndexManager
{
	// TODO: static object initialize order not specified!
	map<int,vector<int>> tablesToTrees;
	map<int,int> treesToTables;
	map<int, Item> idToTree;

	static IndexManager *instance;
public:
	IndexManager();
	~IndexManager();

	static void initialize(const vector<pair<int, AttributeValue>> &rels);

	static Item getRoot(int index)
	{
		return instance->idToTree[index];
	}
};

}

using __index::IndexManager;

}