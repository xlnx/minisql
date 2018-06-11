#include <index/indexManager.h>
#include <index/BPlusTree.h>

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
			tablesToTrees[table_id].push_back(table_id);
			BPlusTree currentTree{ root,attrno };
			idToTree[index_id] = currentTree;


			//insert data
			//
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

		bool IndexManager::insertData(int table_id, const ItemValue &values)
		{
			//
			return true;
		}

		int IndexManager::deleteData(int index_id, const Pair &range, const Filter &filter)
		{
			//
			return 0;
		}

	}

}