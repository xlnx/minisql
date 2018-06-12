#include <index/indexManager.h>
#include <index/bplusTree.h>
#include <stack>

namespace minisql
{

namespace __index
{

IndexManager::IndexManager()
{	// DO nothing.
}

IndexManager::~IndexManager()
{

	// TODO: free
}


void IndexManager::initialize(const vector<pair<int, AttributeValue>> &rels)
{
	// TODO: Accept data and initialize;
	auto it = rels.begin();
	for (int i = 0; it != rels.end(); ++it, ++i) {
		int table = it->first;
		if (table != i && table != SQL_NULL) {
			treesToTables[i] = table;
			//new a bplus tree with std::get<Item>(it->second)
			idToTree[i] = { std::get<Item>(it->second),
				CatalogManager::getIndexAttribute(i) };
			tablesToTrees[table].push_back(i);
		}
	}

	// help me with my map between id and tree!plz!!!!!!!!!
	// ok . it is here.
}

void IndexManager::dropAllIndex(int table_id)
{
	
	vector<int>& trees = tablesToTrees[table_id];
	for (auto index_id: trees)
	{
		idToTree.erase(index_id);
		treesToTables.erase(index_id);
		BufferManager::removeBufferType(index_id);
	}
	tablesToTrees.erase(table_id);
}

Item IndexManager::createNode(BufferType index_id, const ItemValue &value)
{
	auto node = BufferManager::insertItem(index_id, value);
	BufferManager::addRef(node);
	return node;
}

void IndexManager::deleteNode(const Item &node)
{
	BufferManager::removeRef(node);
}

BufferType IndexManager::createIndex(int table_id, Attributeno attrno)
{
	BufferType index_id = BufferManager::registerBufferType({
		SQL_POINTER_DATA,	//data
		SQL_INT,			//count
		SQL_POINTER_NODE,	//son
		SQL_POINTER_NODE	//next
	}, table_id);
	auto root = createNode(index_id, {
		NullType(),
		0,
		NullType(),
		NullType()
	});
	BufferManager::registerRoot(root);//************************!!!!!!!!FBI WARNING!!!!!!********************************

	treesToTables[index_id] = table_id;
	tablesToTrees[table_id].push_back(index_id);
	BPlusTree currentTree{ root,attrno };
	idToTree[index_id] = currentTree;

	auto iid = tablesToTrees[table_id][0];
	auto &tree = idToTree[iid];

	Node head = getHeadNode(tree.root);

	Node a = head;
	while (1) 
	{
		Node b = a.next();
		for (int i = 0; i < a.count(); ++i) 
		{
			insertDataToCreateIndex(b.data(), index_id);
			b = b.next();
		}
		if (b == nullptr)
		{
			break;
		}
		a = b;
	}
	
	return index_id;
}

void IndexManager::dropIndex(int index_id)
{
	//
}

std::vector<Item> IndexManager::queryData(int index_id, const Pair &range, const Filter &Filter)
{
	auto attrno = CatalogManager::getIndexAttribute(index_id);



	// vector<Item>
	// Item a;
	// a[attrno] < range.first && a[attrno]
	return std::vector<Item>();
}

int IndexManager::deleteData(int index_id, const Pair &range, const Filter &filter)
{
	//
	return 0;
}



}

}