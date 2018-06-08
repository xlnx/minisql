#pragma once

#include <debug/debug.hpp>
#include <index/indexManager.h>
#include <buffer/buffer_aux.h>
#include <interpreter/expression.h>
#include <catalog/catalog.h>
#include <string>
#include <map>
#include <vector>

namespace minisql
{

namespace __api
{

struct TableAttribute
{
	std::string name;
	BufferElem type;
	bool isUnique = false;
	bool isPrimary = false;

	friend std::ostream &operator << (std::ostream &os, const TableAttribute &at)
	{
		os << at.name << " " << at.type << " " << at.isUnique << " " << at.isPrimary; return os;
	}
};

class API
{

public:
	static void select(
		const std::string &tableName
	);

	static void select(
		const std::string &tableName, 
		const Expr &cond
	);

	static void select(
		const std::string &tableName, 
		const std::vector<std::string> &row
	);

	static void select(
		const std::string &tableName, 
		const std::vector<std::string> &row,
		const Expr &cond
	);

	static void erase(
		const std::string &tableName
	);

	static void erase(
		const std::string &tableName,
		const Expr &cond
	);

	static void createTable(
		const std::string &tableName,
		const std::vector<TableAttribute> &attrs
	);

	static void dropTable(
		const std::string &tableName
	);

	static void createIndex(
		const std::string &tableName,
		const std::string &attr,
		const std::string &indexName
	);

	static void dropIndex(
		const std::string &indexName
	);

	static void insert(
		const std::string &tableName,
		const std::vector<Value> &values
	);
};

}

using __api::API;
using __api::TableAttribute;

}