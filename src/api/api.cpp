#include <api/api.h>

namespace minisql::__api
{

void API::select(
	const std::string &tableName
)
{
	debug::print::ln("select * from", tableName);
}

void API::select(
	const std::string &tableName, 
	const Expr &cond
)
{
	debug::print::ln("select * from", tableName, "where cond");
}

void API::select(
	const std::string &tableName, 
	const std::vector<std::string> &row
)
{
	debug::print::ln("select row from", tableName);
}

void API::select(
	const std::string &tableName,
	const std::vector<std::string> &row,
	const Expr &cond
)
{
	debug::print::ln("select rows from", tableName, "where cond");
}

void API::erase(
	const std::string &tableName
)
{
	debug::print::ln("delete from", tableName);
}

void API::erase(
	const std::string &tableName,
	const Expr &cond
)
{
	debug::print::ln("delete from", tableName, "where cond");
}

void API::createTable(
	const std::string &table,
	const std::vector<TableAttribute> &attrs
)
{
	auto sec = debug::time([&]() {
		debug::print::ln("create table", table, "attrs");

		std::string pk = "";
		for (auto &e: attrs)
		{
			if (e.isPrimary)
			{
				if (e.isUnique)
					throw "primary key must not be specified 'unique'.";
				if (pk != "")
					throw "duplicate primary key not supported.";
				pk = e.name;
			}
		}

		debug::print::ln(attrs);

		ItemType ty;
		for (auto &e: attrs)
		{
			ty.emplace_back(e.type);
		}

		BufferManager::registerBufferType(ty);


		// debug::print::ln(genAttrs);
		// for (auto &e: attrs)
		// {
		// 	debug::print::ln();
		// }

		// BufferManager::registerBufferType({SQL_CHAR(30), SQL_INT, SQL_INT, SQL_CHAR(10)});
		// BufferManager::registerBufferType({SQL_POINTER_NODE, SQL_POINTER_DATA, SQL_POINTER_NODE}, 0);
	});
	using namespace debug::strutil;
	debug::print::ln("Query OK", "("_s + sec + " sec)");
}

void API::dropTable(
	const std::string &table
)
{
	debug::print::ln("drop table", table);
}

void API::createIndex(
	const std::string &table,
	const std::string &attr,
	const std::string &indexName
)
{
	debug::print::ln("create index", indexName, "on", attr, "of", table);
}

void API::dropIndex(
	const std::string &indexName
)
{
	debug::print::ln("drop index", indexName);
}

void API::insert(
	const std::string &tableName,
	const std::vector<Value> &values
)
{
	debug::print::ln("insert into", tableName);
}

}