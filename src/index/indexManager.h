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
				Node(Item item):item(item) {}
				Item Data();
				int count();
				void writeCount(int a);
				Node son();
				Node next();
			private:
				Item item;
			};
			Node getHeadNode(Node root);
			void insertDataToCreateIndex(Node x);
			void cutNode(Node x);

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