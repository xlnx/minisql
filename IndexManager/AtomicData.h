#pragma once
class AtomicData {
public:
	enum Type {
		INT = 0,
		FLOAT = 1,
		CHAR = 2
	}type;
	unsigned int length;//int->4 char[n]->n float->4
	char*byteArray;

	AtomicData();
	AtomicData(unsigned int type, unsigned int length, const char*byteArray);
	~AtomicData();
};