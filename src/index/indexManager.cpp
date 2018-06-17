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

BufferType IndexManager::createIndex(int table_id, Attributeno attrno)
{
	BufferType index_id = BufferManager::registerBufferType({
		SQL_POINTER_DATA,	//data
		SQL_INT,			//count
		SQL_POINTER_NODE,	//son
		SQL_POINTER_NODE	//next
	}, table_id);
	auto root = createNode(index_id, 
		NullType(),
		0,
		NullType(),
		NullType()
	);
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
	BPlusTree bplusTree = idToTree[index_id];
	Node head = idToTree[index_id].root;
	idToTree.erase(index_id);
	Node data,now;
	do {
		now = head;
		do {
			data = now.next();
			int tot = now.count();
			for (int i = 0; i < tot; ++i) {
				BufferManager::removeRef(data.data());
				data = data.next();
			}
			now = data;
		} while (now != nullptr);
		head = head.son();
	} while (head != nullptr);
	int table_id = treesToTables[index_id];
	treesToTables.erase(index_id);
	vector<int>&trees = tablesToTrees[table_id];
	for (auto it = trees.begin();it!= trees.end();++it)
		if (*it == index_id) {
			trees.erase(it);
			return;
		}
}

std::vector<Item> IndexManager::queryData(int index_id, const Pair &range, const Filter &filter)
{
	BPlusTree bplusTree = idToTree[index_id];
	vector<Item> res;
	Node now = bplusTree.root;
	int tot;
	while (now.son() != nullptr) {
		tot = now.count();
		while (tot--) {
			if (range.first < now.next().data()[bplusTree.attrno].val())
				break;
			now = now.next();
		}
		now = now.son();
	}

	for (;;) {
		tot = now.count();
		now = now.next();
		while (tot--) {
			Item t = now.data();
			if (range.first <= t[bplusTree.attrno].val()) {
				if (t[bplusTree.attrno].val() <= range.second) {
					if (filter(t))
						res.push_back(t);
				}
				else
					goto out;
			}
			now = now.next();
		}
		if (now == nullptr)
			break;
	}
out:
	return res;
}

int IndexManager::deleteData(int index_id, const Pair &range, const Filter &filter)
{
	BPlusTree &bplusTree = idToTree[index_id];
	int table_id = treesToTables[index_id];
	const vector<int> &trees = tablesToTrees[table_id];
	vector<int> attrnos(trees.size());
	vector<Pair> ranges;

	for (auto i = 0; i != trees.size(); ++i)
		attrnos[i] = idToTree[trees[i]].attrno;

	std::stack<Node>stack;
	stack.push(bplusTree.root);
	Node now, data;
	int counter = 0;
	while (true) {
		now = stack.top();
		if (now.son() == nullptr)
			break;
		data = now;
		for (int i = 0; i < now.count(); ++i) {
			if (range.first < data.next().data()[bplusTree.attrno].val())
				break;
			data = data.next();
		}
		stack.push(data.son());
	}

	now = stack.top();
	Node last;
	do {
		last = now;
		data = now.next();
		for (int i = 0; i < now.count(); ++i) {
			auto val = data.data();
			if (range.first <= val[bplusTree.attrno].val()) {
				if (val[bplusTree.attrno].val() > range.second) {
					goto out;
				}
				if (filter(val)) {
					--i;
					now.count(now.count() - 1);
					++counter;
					if (ranges.size() == 0) {
						ranges = vector<Pair>(trees.size());
						for (int i = 0; i < trees.size(); ++i)
							ranges[i] = Pair{ val[attrnos[i]].val(), val[attrnos[i]].val() };
					}
					else {
						for (int i = 0; i < attrnos.size(); ++i) {
							auto t = val[attrnos[i]].val();
							if (t < ranges[i].first)
								ranges[i].first = t;
							if (ranges[i].second < t)
								ranges[i].second = t;
						}
					}
					BufferManager::removeRef(val);
					data = data.next();
					deleteNode(last.next());
					last.next(data);
					continue;
				}
			}
			last = data;
			data = data.next();
		}
		now = data;
	} while (now);
out:
	stack.pop();
	while (!stack.empty()) {
		now = stack.top();
		stack.pop();
		do {
			while (now.next() != nullptr && now.next().count() == -1 && now.son().count() < (M - 1) / 2)
				update_a(now, now, now.next());
			last = now;
			data = now.next();
			int tot = now.count();
			for (int i = 0; i < tot; ++i) {
				if (data.data()[bplusTree.attrno].val() > range.second)
					goto nextFloor;
				else {
					if (data.son().count() < (M - 1) / 2)
						update_b(now, last, data);
					else {
						last = data;
						data = data.next();
					}
				}
			}
			now = data;
		} while (now);
	nextFloor:
		;
	}
	Node root = bplusTree.root;
	while (root.count() == 0 && root.son() != nullptr) {
		Node t = root;
		root = root.son();
		deleteNode(t);
	}
	BufferManager::registerRoot(root);//************************!!!!!!!!FBI WARNING!!!!!!
	bplusTree.root = root;//***************fbi warning,insertingfunctions.cpp

	if (ranges.size()) {
		for (int i = 0; i < trees.size(); ++i) {
			if (trees[i] != index_id) {
				recursivelydelete(idToTree[trees[i]], ranges[i], filter);
			}
		}
	}

	return counter;
	//register root
}

}

}