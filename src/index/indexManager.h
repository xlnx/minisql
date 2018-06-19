#pragma once

#include <buffer/buffer_aux.h>
#include <catalog/catalog.h>
#include <map>
#include <vector>
#include <utility>
#include <functional>

namespace minisql
{

namespace __index
{

using std::map;
using std::vector;
using std::pair;

struct Node: public Item
{
private:
	bool isNull = true;
public:
	Node() = default;
	Node(const Item &item): 
		Item(item), isNull(false) {}

	const bool haveData() const 
	{
		return (*this)[0] != nullptr; 
	}
	const Item data() const 
	{
		return std::get<Item>(BufferManager::readAttribute((*this)[0])); 
	}
	void data(NullType) const
	{
		BufferManager::writeAttribute((*this)[0], NullType());
	}
	void data(std::nullptr_t) const
	{
		BufferManager::writeAttribute((*this)[0], NullType());
	}
	void data(const Item &data) const 
	{
		BufferManager::writeAttribute((*this)[0], data);
	}

	const int count() const 
	{
		return std::get<int>(BufferManager::readAttribute((*this)[1]));
	}
	void count(int a) const 
	{
		BufferManager::writeAttribute((*this)[1], a);
	}

	const Node son() const 
	{
		Attribute son = (*this)[2];
		if (son != nullptr)
		{
			return std::get<Item>(BufferManager::readAttribute(son));
		}
		else
		{
			return Node();
		}
	} 
	void son(const Node &node) const 
	{
		BufferManager::writeAttribute((*this)[2], node);
	} 
		
	const Node next() const 
	{
		Attribute next = (*this)[3];
		if (next != nullptr)
		{
			return std::get<Item>(BufferManager::readAttribute(next));
		}
		else
		{
			return Node();
		}
	} 
	void next(const Node &node) const 
	{
		if (node.isNull)
		{
			BufferManager::writeAttribute((*this)[3], NullType());
		}
		else
		{
			BufferManager::writeAttribute((*this)[3], node);
		}
	}

	operator bool() const
	{
		return !isNull;
	}
	const bool operator == (std::nullptr_t) const
	{
		return isNull;
	}
	const bool operator != (std::nullptr_t) const
	{
		return !isNull;
	}
};

class IndexManager
{
	struct BPlusTree;
	constexpr static int M = 6;

	static map<int, vector<int>> tablesToTrees;
	static map<int, int> treesToTables;
	static map<int, BPlusTree> idToTree;

	typedef AttributeValue Value;
	typedef int Attributeno;
	typedef std::function<bool(const Item&)> Filter;
	typedef std::pair<Value, Value> Pair;

	static Node getHeadNode(Node now)
	{
		while (auto son = now.son())
		{
			now = son;
		}
		return now;
	}
	static void insertDataToCreateIndex(Item x, int index_id);
	static void cutNode(Node x,int index_id);
	static bool recursivelyInsert(
		int &err,
		const ItemValue &values,
		const vector<int> &queue,
		vector<int>::const_iterator &it
	);
	template <typename A, typename B, typename C, typename D>
	static Node createNode(BufferType index_id, const A &a, const B &b,
		const C &c, const D &d)
	{
		Item node;
		if constexpr (std::is_same<D, Node>::value)
		{
			if (d == nullptr)
			{
				node = BufferManager::insertItem(index_id, {
					a, b, c, NullType()
				});
			}
			else
			{
				node = BufferManager::insertItem(index_id, {
					a, b, c, d
				});
			}
		}
		else
		{
			node = BufferManager::insertItem(index_id, {
				a, b, c, d
			});
		}
		BufferManager::addRef(node);
		return node;
	}
	static void deleteNode(const Item &node)
	{
		BufferManager::removeRef(node);
	}

	static void recursivelydelete(
		BPlusTree &bplusTree,
		const Pair &range,
		const Filter &filter
	);
	static void update_a(Node dumb, Node a, Node b);
	static void update_b(Node dumb, Node &a, Node &b);
	static void mergeLeaves(Node &dumb, Node &a, Node &b, Node &ason, Node &bson);
	static void mergeNodes(Node &dumb, Node &a, Node &b, Node &ason, Node &bson);
public:
	IndexManager();
	~IndexManager();

	static void initialize(const vector<pair<int, AttributeValue>> &rels);

	//table operation
	//static void createTable();
	//static void dropTable(int table_id);
	static void dropAllIndex(int table_id);

	//index operation
	static BufferType createIndex(int table_id, Attributeno attrno);
	static void dropIndex(int index_id);

	//data operation
	static std::vector<Item> queryData(int index_id, const Pair &range, const Filter &Filter);
	static bool insertData(int table_id, const ItemValue &values, int &err);
	static int deleteData(int index_id, const Pair &range, const Filter &filter);
};

}

using __index::IndexManager;

}