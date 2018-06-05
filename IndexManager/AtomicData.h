#pragma once
class AtomicData {
public:

	short type;

	enum Type {
		INT = 0,
		FLOAT = 1,
		CHAR = 2
	}type;
	unsigned int length;//int->4 char[n]->n float->4
	//**************************************************************debug
	char*byteArray;

	AtomicData();
	AtomicData(unsigned int type, unsigned int length, const char*byteArray);
	~AtomicData();
};