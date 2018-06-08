#include <index/indexManager.h>

namespace minisql
{

namespace __index
{

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
		if (table != i) {
			treesToTables[i] = table;
			idToTree[i] = std::get<Item>(it->second);
			tablesToTrees[table].push_back(i);
		}
	}

	// help me with my map between id and tree!plz!!!!!!!!!
	// ok . it is here.
}


std::vector<Item> IndexManager:: query(int id, const Pair &range, const Filter &Filter)
{
	auto attrno = CatalogManager::lookup(id);

	

	// vector<Item>
	// Item a;
	// a[attrno] < range.first && a[attrno]
}


Item [0, 1, 2]


}

}