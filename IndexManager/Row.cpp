#include"Row.h"
Row::Row() {
	length = 0;
	atomicDataArray = nullptr;
}
Row::Row(unsigned int length) {
	atomicDataArray = new AtomicData*[length];
	for (int i = 0; i < length; ++i)
		atomicDataArray[i] = nullptr;
	this->length = length;
}
Row::~Row() {
	if (length != 0) {
		for (int i = 0; i < length; ++i)
			if (atomicDataArray[i] != nullptr)
				delete atomicDataArray[i];
		delete[]atomicDataArray;
	}
}