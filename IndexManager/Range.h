#pragma once
#include"Row.h"
struct Range {
	Row LeftBoundIndex;
	Row RightBoundIndex;

	short Infinite;//00->(,) 01->(,] 10->[,) 11[]
};