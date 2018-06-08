#pragma once

#include <buffer/buffer_aux.h>
#include <map>
#include <vector>
#include <utility>
#include <function>

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

	typedef std::function<bool(const Value&)> Filter;
	typedef std::pair<Value, Value> Pair;
public:
	IndexManager();
	~IndexManager();

	static void initialize(const vector<pair<int, AttributeValue>> &rels);
	
	static std::vector<Item> query(int id, const Pair &range, const Filter &Filter);

	// static Item getRoot(int index)
	// {
	// 	return idToTree[index];
	// }
};

}

using __index::IndexManager;

}