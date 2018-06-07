#include <index/indexManager.h>

namespace minisql
{

namespace __index
{

IndexManager *IndexManager::instance;

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
	static IndexManager man;
	instance = &man;
	// TODO: Accept data and initialize;
	auto it = rels.begin();
	for (int i = 0; it != rels.end(); ++it,++i) {
		int table = it->first;
		if (table != i){
			instance->treesToTables[i] = table;
			instance->idToTree[i] = std::get<Item>(it->second);
			instance->tablesToTrees[table].push_back(i);
		}
	}

	// help me with my map between id and tree!plz!!!!!!!!!
	// ok . it is here.
}

}

}