#include <index/indexManager.h>

namespace minisql
{

namespace __index
{

	map<int, vector<int>> IndexManager::tablesToTrees;
	map<int, int> IndexManager::treesToTables;
	map<int, BPlusTree*> IndexManager::idToTree;

static IndexManager dummy;

IndexManager::IndexManager()
{
	// TODO: init;
}

IndexManager::~IndexManager()
{
	// TODO: free
}


void IndexManager::initialize(const vector<int> &rels)
{
	// TODO: Accept data and initialize;
	vector<int>::const_iterator it = rels.begin();
	for (int i = 0; it != rels.end(); it = it + 1,++i) {
		int table = *it;
		if (table != i){
			treesToTables[i] = table;
			tablesToTrees[table].push_back(i);
		}
	}

	//help me with my map between id and tree!plz!!!!!!!!!
}

}

}