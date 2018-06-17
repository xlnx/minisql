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

class API
{
	struct VarRange
	{
		std::vector<const Value*> eq;
		std::vector<const Value*> neq;
		std::pair<const Value*, const Value*> range
			= std::make_pair(nullptr, nullptr);
	};

	static void triv(
		const Expr &e, 
		std::map<std::string, VarRange> &range,
		bool &eval
	);

	static BufferType cond2RangeFilter(
		const ItemType &itemType,
		std::map<std::string, std::pair<BufferType, SizeType>> &indexes,
		const std::vector<std::string> &attrs,
		const Expr &cond,
		bool &eval,
		std::pair<AttributeValue, AttributeValue> &range,
		std::function<bool(const Item &)> &filter
	);
public:
	static bool doPrint;

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

	static void showTables();

	static void showIndexs(
		const std::string &tableName
	);
};

}

using __api::API;

}