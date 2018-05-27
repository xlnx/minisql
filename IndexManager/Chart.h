#pragma once
#include<vector>
#include<string>
#include"Row.h"
using std::vector;
using std::string;
struct Chart{
	//save the name of the attributes in order
	vector<string>attributes;
	vector<Row>rows;
	/*----------------------------------------------------*\
	 |      attribute a | attribute b | attribute c | ... |
	 |row a     xx            yy            zz            |
	 |row b     xx            yy            zz            |
	 |row c     xx            yy            zz            |
	 |...                                                 |
	\*----------------------------------------------------*/
};