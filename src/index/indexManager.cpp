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
		if (table != i) {
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
	// TODO: drop all indexs.
}

BufferType IndexManager::createIndex(int table_id, Attributeno attrno)
{
	BufferType index_id = BufferManager::registerBufferType({
		SQL_POINTER_DATA,	//data
		SQL_INT,			//count
		SQL_POINTER_NODE,	//son
		SQL_POINTER_NODE	//next
	}, table_id);
	auto root = BufferManager::insertItem(index_id, {
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

bool IndexManager::insertData(int table_id, const ItemValue &values, int &err)
{
	vector<int> &targetTrees = tablesToTrees[table_id];
	vector<int>::const_iterator it = targetTrees.begin();
	return recursivelyInsert(table_id, err, values, targetTrees, it);
}

int IndexManager::deleteData(int index_id, const Pair &range, const Filter &filter)
{
	//
	return 0;
}

bool IndexManager::recursivelyInsert(
	int table_id,
	int &err,
	const ItemValue &values,
	const vector<int> &queue,
	vector<int>::const_iterator &it)
{
	int index_id = *it;
	std::stack<Node> Stack;
	BPlusTree bplusTree = idToTree[index_id];
	Node root{ bplusTree.root };
	Stack.push(root);
	Node now, data;
	//const ItemValue &value=x[bplusTree.attrno];

	while (true) {
		now = Stack.top();
		if (now == nullptr)
			break;
		data = now;
		for (int i = 0; i < now.count(); ++i) {
			//if (value < xxxxxxxxxxxx)
			auto next = data.next();
			if (values[bplusTree.attrno] < next.data()[bplusTree.attrno].val())
				break;
			data = next;
		}
		Stack.push(data);
		Stack.push(data.son());
	}
	Stack.pop();

	now = Stack.top();
	if (now.haveData() && now.data()[bplusTree.attrno].val() == values[bplusTree.attrno]) {
		//roll back******any thing else?
		err = bplusTree.attrno;
		return false;
	}

	static Item x;
	if (++it == queue.end())
		x = BufferManager::insertItem(table_id, values);
	else
		if (!recursivelyInsert(table_id, err, values, queue, it))//recursively insert
			return 0;

	now.next(BufferManager::insertItem(index_id, {
		x,//just a pointer to real data
		-1,
		NullType(),
		now.next()
	}));
	Stack.pop();
	now = Stack.top();
	now.count(now.count() + 1);
	Stack.pop();

	while (!Stack.empty()) {
		data = Stack.top();
		Stack.pop();
		now = Stack.top();
		Stack.pop();
		if (data.son().count() == M) {
			cutNode(data, index_id);
			now.count(now.count());
		}
	}
	if (root.count() == M) {
		root = BufferManager::insertItem(index_id, {
			NullType(),
			1,
			root,
			NullType()
		});
		cutNode(root, index_id);

		BufferManager::registerRoot(root);//************************!!!!!!!!FBI WARNING!!!!!!
	}
	return true;
}

}

}