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
	const std::map<std::string, TableAttribute> &attrs
)
{
	debug::print::ln("create table", table, "attrs");
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