#pragma once
#include<string>
#include<vector>
using std::string;
using std::vector;

class Subcondition {
public:
	//if attributionA==attributionB, the condition is in format "attribute op value"
	//or it must be in format "attribute op attribute"
	std::string attributeA;
	std::string attributeB;
	int A, B;
	enum Operation {
		LESS = 0,
		NOMORE = 1,
		EQUAL = 2,
		NOLESS = 3,
		MORE = 4
	}operation;

	int length;
	char*value;

	Subcondition();
	Subcondition(string A, string B, unsigned int operation, const char* value, int length);

	~Subcondition();
	//bool calculate(const Row* row)const {}
};

class Condition {
public:
	vector<Subcondition*>subconditionArray;

	~Condition();
	//bool calculate(const Row* row)const {}
};