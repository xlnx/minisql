#pragma once
#include"Row.h"
struct Node;
struct ListUnit {
	Row*data;
	ListUnit*next;
	Node*son;
};

struct Node {
	Node*fatherNode;
	//ListUnit*fatherListUnit;
	bool type;
	ListUnit head;
	int count;
};