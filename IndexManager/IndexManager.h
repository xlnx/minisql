#pragma once
#include<string>
#include<vector>
#include"Condition.h"
#include"Range.h"
#include"Chart.h"

/*
  create table
  drop table

  ¡Ìselect
  ¡Ìdelete
  insert

  create index
  drop index
*/
class IndexManager {
public:
	static bool createTable(string table,const vector<string>&index,const Row&attribute);
	static bool dropTable(string table);

	static Chart* selectWithWholeTable(string table, const Condition& condition);
	static Chart* selectWithRange(string table, const Range&range, const Condition& condition);
	static DeleteResult* deleteWithWholeTable(string table, const Condition& condition);
	static DeleteResult* deleteWithRange(string table, const Range&range, const Condition& condition);
	static bool insert(string table,const Chart& value);

	static bool createIndex(string table, string indexName, const Row&index);

};