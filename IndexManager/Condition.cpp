#include"Condition.h"

Subcondition::Subcondition() {
	length = 0;
	value = nullptr;
}
Subcondition::Subcondition(string A, string B, unsigned int operation, const char* value, int length) {
	attributeA = A;
	attributeB = B;
	this->operation = (Operation)operation;
	if (length != 0) {
		value = new char[length];
		for (int i = 0; i < length; ++i)
			this->value[i] = value[i];
	}
}

Subcondition::~Subcondition() {
	if (length != 0)
		delete[]value;
}

Condition::~Condition() {
	vector<Subcondition*>::iterator i;
	for (i = subconditionArray.begin(); i != subconditionArray.end(); i++)
		delete *i;
}