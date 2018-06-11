#pragma once 

#include <index/indexManager.h>

namespace minisql
{

namespace __index
{

struct IndexManager::BPlusTree 
{
	/*
	struct Node {
	Data data;
	int count;
	Node*son, *next;
	Node(Data data, Node*son, Node*next) :
	data(data),
	son(son),
	next(next),
	count(-1)
	{}

	Node(Data data, Node*son, Node*next,int count) :
	data(data),
	son(son),
	next(next),
	count(count)
	{}
	};
	*/
	Item root;
	Attributeno attrno;
};

}

}