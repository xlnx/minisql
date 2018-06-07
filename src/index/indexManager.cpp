#include <index/indexManager.h>

namespace minisql
{

namespace __index
{

	map<int, vector<int>> IndexManager::tablesToTrees;
	map<int, int> IndexManager::treesToTables;
	map<int, Item> IndexManager::idToTree;

static IndexManager dummy;

IndexManager::IndexManager()
{
	// TODO: init;
}

IndexManager::~IndexManager()
{
	// TODO: free
}


void IndexManager::initialize(const vector<pair<int, AttributeValue>> &rels)
{
	// TODO: Accept data and initialize;
	auto it = rels.begin();
	for (int i = 0; it != rels.end(); ++it,++i) {
		int table = it->first;
		if (table != i){
			treesToTables[i] = table;
			idToTree[i] = std::get<Item>(it->second);
			tablesToTrees[table].push_back(i);
		}
	}

	// help me with my map between id and tree!plz!!!!!!!!!
	// ok . it is here.
}

}

}