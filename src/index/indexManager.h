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

		class IndexManager
		{
			struct BPlusTree;
			constexpr static int M = 10;
			static map<int, vector<int>> tablesToTrees;
			static map<int, int> treesToTables;
			static map<int, BPlusTree> idToTree;

			typedef AttributeValue Value;
			typedef int Attributeno;
			typedef std::function<bool(const Item&)> Filter;
			typedef std::pair<Value, Value> Pair;

			struct Node {
				Node() = default;
				Node(Item item): item(item) {}

				Item data() 
					{ return *item[0]; }
				void writeData(Item a)
					{ BufferManager::writeAttribute(item[0], a); }
				int count()
					{ return std::get<int>(BufferManager::readAttribute(item[1])); }
				void writeCount(int a)
					{ BufferManager::writeAttribute(item[1], a); }
				Node son()
					{ return Node(*item[2]); }
				void writeSon(Item a)
					{ BufferManager::writeAttribute(item[2], a); }
				Node next()
					{ return Node(*item[3]); }
				void writeNext(Item a)
					{ BufferManager::writeAttribute(item[3], a); }
					
				Item item;
			};
			static Node getHeadNode(Node now)
			{
				while (now.item[2] != nullptr)
					now = now.son();
				return now;
			}
			static void insertDataToCreateIndex(Item x,const BPlusTree &bplusTree);
			static void cutNode(Node x,int index_id);

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
			static bool insertData(int table_id, const ItemValue &values);
			static int deleteData(int index_id, const Pair &range, const Filter &filter);
		};

	}

	using __index::IndexManager;

}