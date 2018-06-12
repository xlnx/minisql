#pragma once

#include <buffer/buffer_def.h>
#include <buffer/buffer_aux.h>
#include <string>

namespace minisql
{

namespace __catalog
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

struct TableInfo
{
	BufferType type;
	std::vector<TableAttribute> attrs;
};

struct IndexInfo
{
	BufferType table;
	BufferType type;
	SizeType attrno;
};

struct IndexProperty
{
	std::string name;
	BufferType index_id;
	SizeType attrno;
};

class CatalogManager
{
	static std::map<std::string, TableInfo> tableInfos;
	static std::map<std::string, IndexInfo> indexInfos;
	static std::fstream icursor;
public:
	CatalogManager();
	~CatalogManager();

	static void registerTableInfo(
		BufferType table_id, 
		const std::string &tableName, 
		const std::vector<TableAttribute> &attrs
	);
	static void removeTableInfo(const std::string &tableName);
	static bool tableExist(const std::string &tableName);
	static BufferType getTableId(const std::string &tableName);
	static std::vector<std::string> getAttributeNames(const std::string &tableName);
	static std::vector<TableAttribute> getAttributeProperties(const std::string &tableName);
	static std::vector<std::string> getTableNames();

	static void registerIndexInfo(
		BufferType table_id,
		BufferType index_id,
		SizeType attrno,
		const std::string &indexName
	);
	static void removeIndexInfo(const std::string &indexName);
	static bool indexExist(const std::string &indexName);
	static BufferType getIndexId(const std::string &indexName);
	static SizeType getIndexAttribute(BufferType id);
	static std::vector<std::string> getIndexNames();
	static std::vector<IndexProperty> getIndexProperties(const std::string &tableName);
};

}

using __catalog::CatalogManager;
using __catalog::TableAttribute;
using __catalog::IndexProperty;

}