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
	static map<int,vector<int>> tablesToTrees;
	static map<int,int> treesToTables;
	static map<int, Item> idToTree;
public:
	IndexManager();
	~IndexManager();

	static void initialize(const vector<pair<int, AttributeValue>> &rels);
};

}

using __index::IndexManager;

}