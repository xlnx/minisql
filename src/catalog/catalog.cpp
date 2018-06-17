#include <catalog/catalog.h>
#include <interpreter/interpreter_aux.h>
#include <debug/debug.hpp>
#include <jsoncpp/json.h>
#include <buffer/buffer_aux.h>
#include <fstream>

namespace minisql::__catalog
{

CatalogManager::CatalogManager()
{
	auto &fs = icursor;
	fs.seekg(0, std::ios::beg);
	int size;
	fs.read(reinterpret_cast<char*>(&size), sizeof(int));
	auto s = new char[size + 1]; s[size] = 0;
	fs.read(s, size);
	Json::Reader reader;
	// debug::print::ln(s);
	Json::Value root;
	reader.parse(s, root);
	delete [] s;
	auto &table = root["table"];
	for (auto itr = table.begin(); itr != table.end(); ++itr)
	{
		auto &e = *itr;
		std::vector<TableAttribute> attrs;
		for (auto &f: e["attrs"])
		{
			TableAttribute attr;
			attr.name = f["name"].asString();
			attr.type = BufferType(f["type"].asUInt());
			attr.isUnique = f["isUnique"].asBool();
			attr.isPrimary = f["isPrimary"].asBool();
			attrs.emplace_back(std::move(attr));
		}
		tableInfos.emplace(
			itr.key().asString(),
			TableInfo {
				BufferType(e["type"].asUInt()),
				attrs
			}
		);
	}
	auto &index = root["index"];
	for (auto itr = index.begin(); itr != index.end(); ++itr)
	{
		auto &e = *itr;
		indexInfos[itr.key().asString()] = IndexInfo {
			BufferType(e["table"].asUInt()),
			BufferType(e["type"].asUInt()),
			SizeType(e["attrno"].asUInt())
		};
	}
}

CatalogManager::~CatalogManager()
{
	Json::Value root(Json::objectValue);
	for (auto &e: tableInfos)
	{
		auto &v = root["table"][e.first] = Json::objectValue;
		v["type"] = e.second.type;
		auto &attrs = v["attrs"] = Json::arrayValue;
		for (auto &f: e.second.attrs)
		{
			auto &attr = attrs.append(Json::objectValue);
			attr["name"] = f.name;
			attr["type"] = f.type;
			attr["isUnique"] = f.isUnique;
			attr["isPrimary"] = f.isPrimary;
		}
	}
	for (auto &e: indexInfos)
	{
		auto &v = root["index"][e.first] = Json::objectValue;
		v["table"] = e.second.table;
		v["type"] = e.second.type;
		v["attrno"] = e.second.attrno;
	}
	Json::
		FastWriter 
		// StyledWriter
	writer;
	auto src = writer.write(root);
	// debug::print::ln(src);
	auto &fs = icursor;
	fs.seekp(0, std::ios::beg);
	int size = src.length();
	fs.write(reinterpret_cast<const char*>(&size), sizeof(int));
	fs.write(src.c_str(), size);
}

void CatalogManager::registerTableInfo(
	BufferType table_id, 
	const std::string &tableName, 
	const std::vector<TableAttribute> &attrs
)
{
	tableInfos[tableName].type = table_id;
	tableInfos[tableName].attrs = attrs;
}

BufferType CatalogManager::getTableId(const std::string &tableName)
{
	if (tableInfos.count(tableName))
	{
		return tableInfos[tableName].type;
	}
	else
	{
		throw InterpretError("table does not exist: " + tableName);
	}
}

bool CatalogManager::tableExist(const std::string &tableName)
{
	return tableInfos.count(tableName);
}

std::vector<std::string> CatalogManager::getAttributeNames(const std::string &tableName)
{
	std::vector<std::string> vec;
	for (auto &e: tableInfos[tableName].attrs)
	{
		vec.emplace_back(e.name);
	}
	return vec;
}

std::vector<std::string> CatalogManager::getTableNames()
{
	std::vector<std::string> names;
	for (auto &e: tableInfos) names.emplace_back(e.first);
	return names;
}

void CatalogManager::registerIndexInfo(
	BufferType table_id,
	BufferType index_id,
	SizeType attrno,
	const std::string &indexName
)
{
	indexInfos[indexName].table = table_id;
	indexInfos[indexName].type = index_id;
	indexInfos[indexName].attrno = attrno;
}

void CatalogManager::removeTableInfo(const std::string &tableName)
{
	if (tableInfos.count(tableName))
	{
		auto itr = tableInfos.find(tableName);
		tableInfos.erase(itr);
	}
	else
	{
		throw InterpretError("table does not exist: " + tableName);
	}
}

void CatalogManager::removeIndexInfo(const std::string &indexName)
{
	if (indexInfos.count(indexName))
	{
		auto itr = indexInfos.find(indexName);
		indexInfos.erase(itr);
	}
	else
	{
		throw InterpretError("index does not exist: " + indexName);
	}
}

bool CatalogManager::indexExist(const std::string &tableName)
{
	return indexInfos.count(tableName);
}

BufferType CatalogManager::getIndexId(const std::string &indexName)
{
	if (indexInfos.count(indexName))
	{
		return indexInfos[indexName].type;
	}
	else
	{
		throw InterpretError("index does not exist: " + indexName);
	}
}

SizeType CatalogManager::getIndexAttribute(BufferType index_id)
{
	for (auto &e: indexInfos)
	{
		if (e.second.type == index_id)
		{
			return e.second.attrno;
		}
	}
	throw InterpretError("internal error");
}

std::vector<std::string> CatalogManager::getIndexNames()
{
	std::vector<std::string> vec;
	for (auto &e: indexInfos) vec.emplace_back(e.first);
	return vec;
}

std::vector<IndexProperty> CatalogManager::getIndexProperties(const std::string &tableName)
{
	std::vector<IndexProperty> vec;
	auto table_id = getTableId(tableName);
	for (auto &e: indexInfos)
	{
		if (e.second.table == table_id)
		{
			IndexProperty p;
			p.name = e.first;
			p.index_id = e.second.type;
			p.attrno = e.second.attrno;
			vec.emplace_back(p);
		}
	}
	return vec;
}

std::vector<TableAttribute> CatalogManager::getAttributeProperties(const std::string &tableName)
{
	if (tableInfos.count(tableName))
	{
		return tableInfos[tableName].attrs;
	}
	else
	{
		throw InterpretError("table does not exist: " + tableName);
	}
}

}