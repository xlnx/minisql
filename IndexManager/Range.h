#pragma once
#include"Row.h"
struct Range {
	Row LeftBoundIndex;
	Row RightBoundIndex;

	bool leftInfinite;
	bool includeLeft;
	
	bool rightInfinite;
	bool includeRight;
};