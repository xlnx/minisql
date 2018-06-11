#include <index/indexManager.h>
#include <index/bplusTree.h>
#include<stack>
namespace minisql
{

	namespace __index
	{

		void IndexManager::cutNode(Node data, int index_id) {
			Node now = data.son();
			Node tail = now;
			now.writeCount(M >> 1);
			for (int i = 0; i < M / 2; ++i)
				tail = tail.next();
			if (not.item[2] == nullptr) {
				tail.writeNext(BufferManager::insertItem(index_id, {
					NullType(),
					(M + 1) >> 1,
					NullType(),
					(Item)tail.next()
					}));
				data.writeNext(BufferManager::insertItem(index_id, {
					tail.next().next().data(),
					-1,
					(Item)tail.next(),
					(Item)data.next()
					}));
			}
			else {
				data.writeNext(BufferManager::insertItem(index_id, {
					tail.next().data(),
					-1,
					(Item)tail.next(),
					(Item)data.next()
					}));
				tail.next().writeCount((M - 1) >> 1);
				tail.next().writeData(0);
			}
		}
		void IndexManager::insertDataToCreateIndex(Item x, const BPlusTree &bplusTree) {
			std::stack<Node>Stack;
			Node root{ bplusTree.root };
			Stack.push(root);
			Node now, data;
			//const ItemValue &value=x[bplusTree.attrno];

			while (true) {
				now = Stack.top();
				if (now == NullType())
					break;
				data = now;
				for (int i = 0; i < now.count(); ++i) {
					//if (value < xxxxxxxxxxxx)
					if (x[bplusTree.attrno] < data.next().data()[bplusTree.attrno])
						break;
					data = data.next();
				}
				Stack.push(data);
				Stack.push(data.son());
			}
			Stack.pop();

			now = Stack.top();

			now.writeNext(BufferManager::insertItem(index_id, {
				x,//just a pointer to real data
				-1,
				NullType(),
				(Item)now.next()
				}));
			Stack.pop();
			now = Stack.top();
			now.writeCount(now.count() + 1);
			Stack.pop();

			while (!Stack.empty()) {
				data = Stack.top();
				Stack.pop();
				now = Stack.top();
				Stack.pop();
				if (data.son().count() == M) {
					cutNode(data,index_id);
					now.writeCount(now.count());
				}
			}
			if (root.count() == M) {
				root = BufferManager::insertItem(index_id, {
					NullType(),
					1,
					(Item)root,
					NullType()
					});
				cutNode(root,index_id);
			}
		}
	}
}