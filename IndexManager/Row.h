#pragma once
#include"AtomicData.h"
class Row {
public:
	unsigned int length;
	AtomicData**atomicDataArray;

	Row();
	Row(unsigned int length);
	~Row();
};