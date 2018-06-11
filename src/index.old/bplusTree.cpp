#include <index/indexManager.h>
namespace minisql
{

	namespace __index
	{

		Item IndexManager::Node::Data() { return Item(*item[0]); }
		int IndexManager::Node::count() { return std::get<int>(BufferManager::readAttribute(item[1])); }
		void IndexManager::Node::writeCount(int a) { BufferManager::writeAttribute(item[1], a); }
		IndexManager::Node IndexManager::Node::son() { return Node(*item[2]); }
		IndexManager::Node IndexManager::Node::next() { return Node(*item[3]); }
		

		IndexManager::Node IndexManager::getHeadNode(Node now) {
			// while (now.item[2] != nullptr)
			// 	now = now[2];
			// return now;
		}
		void IndexManager::cutNode(Node data) {
			// Node now = data.son();
			// Node tail = now;
			// now.writeCount(M >> 1);
			// for (int i = 0; i < M / 2; ++i)
			// 	tail = tail.next();
			// if (now->son == nullptr) {
			// 	tail->next = new Node{ 0,nullptr,tail->next,(M + 1) >> 1 };
			// 	data->next = new Node{ tail->next->next->data,tail->next,data->next };
			// }
			// else {
			// 	data->next = new Node{ tail->next->data,tail->next,data->next };
			// 	tail->next->count = (M - 1) >> 1;
			// 	tail->next->data = 0;
			// }
		}
		void IndexManager::insertDataToCreateIndex(Node x) {

		}
	}
}