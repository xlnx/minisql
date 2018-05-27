#include"AtomicData.h"

AtomicData::AtomicData() {
	length = 0;
	byteArray = nullptr;
}
AtomicData::AtomicData(unsigned int type, unsigned int length, const char*byteArray) {
	this->type = (Type)type;
	this->length = length;
	this->byteArray = new char[length];
	for (int i = 0; i < length; ++i)
		this->byteArray[i] = byteArray[i];
}
AtomicData::~AtomicData() {
	if (length != 0)
		delete[]this->byteArray;
}