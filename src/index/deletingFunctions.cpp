#include "indexManager.h"
#include "bplusTree.h"
#include <stack>
#include <set>

namespace minisql
{

namespace __index
{

void IndexManager::recursivelydelete(
	BPlusTree &bplusTree,
	const Pair &range,
	const Filter &filter
){
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
	BufferManager::registerRoot(root);
	bplusTree.root = root;
}

void IndexManager::update_a(Node dumb, Node a, Node b)
{
	Node ason = a.son(), bson = b.son();
	if (ason.son() != nullptr) {
		if (ason.count() + bson.count() + 1 < M)
			mergeNodes(dumb, a, b, ason, bson);
		else {
			int delta = (M - 1) / 2 - ason.count();
			ason.count((M - 1) / 2);
			int tot = bson.count() - delta;
			bson.data(b.data());
			bson.count(-1);
			for (int i = 0; i < delta; ++i)
				bson = bson.next();
			b.son(bson);
			b.data(bson.data());
			bson.data(nullptr);
			bson.count(tot);
		}
	}
	else {
		if (ason.count() + bson.count() < M)
			mergeLeaves(dumb, a, b, ason, bson);
		else {
			int delta = (M - 1) / 2 - ason.count();
			ason.count((M - 1) / 2);
			Node t = bson.next();
			b.son(t);
			t.count(bson.count() - delta);
			bson.data(t.data());
			t.data(nullptr);
			bson.count(-1);
			bson.next(t.next());
			for (int i = 1; i < delta; ++i)
				bson = bson.next();
			t.next(bson.next());
			bson.next(t);
			b.data(t.next().data());//add ref to data and delete ref to data
		}
	}
}

void IndexManager::update_b(Node dumb, Node &a, Node &b)
{
	Node ason = a.son(), bson = b.son();
	if (ason.son() != nullptr) {
		if (ason.count() + bson.count() + 1 < M) {
			mergeNodes(dumb, a, b, ason, bson);
			b = a.next();
		}
		else {
			int delta = (M - 1) / 2 - bson.count();
			//bson->count = (M - 1) / 2;
			int tot = ason.count() - delta;
			ason.count(tot);
			bson.count(-1);
			bson.data(b.data());
			for (int i = 0; i < tot; ++i)
				ason = ason.next();
			ason = ason.next();

			b.data(ason.data());
			b.son(ason);
			ason.data(nullptr);
			ason.count((M - 1) / 2);

			a = b;
			b = b.next();
		}
	}
	else {
		if (ason.count() + bson.count() < M) {
			mergeLeaves(dumb, a, b, ason, bson);
			b = a.next();
		}
		else {
			int delta = (M - 1) / 2 - bson.count();
			bson.count((M - 1) / 2);
			int tot = ason.count() - delta;
			ason.count(tot);
			for (int i = 0; i < tot; ++i)
				ason = ason.next();
			Node temp = ason.next();
			ason.next(bson);
			ason = temp;
			for (int i = 1; i < delta; ++i)
				ason = ason.next();

			ason.next(bson.next());
			bson.next(temp);
			BufferManager::addRef(temp.data());
			BufferManager::removeRef(b.data());
			b.data(temp.data());//delete and add ref

			a = b;
			b = b.next();
		}
	}
}

void IndexManager::mergeLeaves(Node &dumb, Node &a, Node &b, Node &ason, Node &bson)
{
	dumb.count(dumb.count() - 1);
	if (bson.count()) {
		ason.count(ason.count() + bson.count());
		Node next = bson.next();
		bson.data(next.data());
		bson.next(next.next());
		deleteNode(next);
		a.next(b.next());
	}
	else {
		int t = ason.count();
		for (int i = 0; i < t; ++i)
			ason = ason.next();
		ason.next(bson.next());
		a.next(b.next());
		deleteNode(bson);
	}
	deleteNode(b);
}

void IndexManager::mergeNodes(Node &dumb, Node &a, Node &b, Node &ason, Node &bson)
{
	dumb.count(dumb.count() - 1);
	ason.count(ason.count() + bson.count() + 1);
	bson.data(b.data());
	bson.count(bson.count() - 1);
	a.next(b.next());
	deleteNode(b);
}

}

}