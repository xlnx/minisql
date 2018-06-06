#pragma once

#include <index/bplusTree.h>
#include <map>
#include<vector>
#include <utility>

namespace minisql
{

namespace __index
{

using std::map;
using std::vector;

class IndexManager
{
	static map<int,vector<int>> tablesToTrees;
	static map<int,int> treesToTables;
	static map<int, BPlusTree*> idToTree;
public:
	IndexManager();
	~IndexManager();

	static void initialize(const vector<int> &rels);
};

}

using __index::IndexManager;

}