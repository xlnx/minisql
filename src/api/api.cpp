#include <api/api.h>
#include <interpreter/interpreter_aux.h>
#include <iomanip>

namespace minisql::__api
{

class Sheet
{
	std::vector<std::vector<std::string>> rows;
public:
	Sheet() = default;
	~Sheet() = default;

	std::vector<std::string> &createRow()
	{
		rows.emplace_back();
		return rows.back();
	}

	friend std::ostream &operator << (std::ostream &os, const Sheet &sheet)
	{
		std::vector<int> spaces(sheet.rows[0].size(), 0);
		for (auto i = 0; i != sheet.rows[0].size(); ++i)
		{
			for (auto j = 0; j != sheet.rows.size(); ++j)
			{
				if (sheet.rows[j][i].length() > spaces[i])
				{
					spaces[i] = sheet.rows[j][i].length();
				}
			}
		}
		for (auto i = 0; i != sheet.rows[0].size(); ++i)
		{
			os << "+"; 
			for (auto j = 0; j != spaces[i] + 2; ++j)
			{
				os << "-";
			}
		}
		os << "+\n";
		for (auto j = 0; j != sheet.rows.size(); ++j)
		{
			for (auto i = 0; i != sheet.rows[0].size(); ++i)
			{
				os << "| " << std::left << std::setfill(' ') << 
					std::setw(spaces[i]) << sheet.rows[j][i] << " ";
			}
			os << "|\n";
			if (j == 0)
			{
				for (auto k = 0; k != sheet.rows[0].size(); ++k)
				{
					os << "+"; 
					for (auto h = 0; h != spaces[k] + 2; ++h)
					{
						os << "-";
					}
				}
				os << "+\n";
			}
		}
		for (auto i = 0; i != sheet.rows[0].size(); ++i)
		{
			os << "+"; 
			for (auto j = 0; j != spaces[i] + 2; ++j)
			{
				os << "-";
			}
		}
		os << "+";
		return os;
	}

	int size() const
	{
		return rows.size();
	}
};

void API::select(
	const std::string &tableName
)
{
	// debug::print::ln("select * from", tableName);
	int n;
	auto sec = debug::time([&]{
		Sheet s;
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		for (auto &str: CatalogManager::getAttributeNames(tableName))
		{
			s.createRow().emplace_back(str);
		}
		auto props = CatalogManager::getIndexProperties(tableName);
		auto types = BufferManager::getItemType(table_id);
		for (auto &e: props)
		{
			auto type = e.index_id;
		}
		BufferType index_id = 0;
		std::pair<AttributeValue, AttributeValue> range;
		auto filter = [](Item item) -> bool
		{
			return true;
		};
		for (auto &e: IndexManager::queryData(index_id, range, filter))
		{
			auto &row = s.createRow();
			auto val = BufferManager::readItem(e);
			for (int i = 0; i != type.size(); ++i)
			{
				switch ((type[i] & 0xff0000) >> 16)
				{
					using namespace debug::strutil;
					case 0x10: {		// SQL_INT
						row.emplace_back(""_s + std::get<int>(val[i]));
					} break;
					case 0x20: {
						row.emplace_back(std::get<std::string>(val[i]));
					} break;
					case 0x40: {
						row.emplace_back(""_s + std::get<float>(val[i]));
					} break;
				}
			}
		}
		n = s.size() - 1;
		std::cout << s << std::endl;
	});
	using namespace debug::strutil;
	debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
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
	auto sec = debug::time([&] 
	{
		std::string pk = "";
		int pkid = -1;
		for (auto i = 0; i != attrs.size(); ++i)
		{
			if (attrs[i].isPrimary)
			{
				if (attrs[i].isUnique)
					throw InterpretError("primary key must not be specified 'unique'.");
				if (pk != "")
					throw InterpretError("duplicate primary key not supported.");
				pk = attrs[i].name;
				pkid = i;
			}
		}

		if (pk == "")
		{
			throw InterpretError("primary key is essential.");
		}

		ItemType ty;
		for (auto &e: attrs)
		{
			ty.emplace_back(e.type);
		}

		auto t = BufferManager::registerBufferType(ty);
		CatalogManager::registerTableInfo(t, table, attrs);
		auto u = IndexManager::createIndex(t, pkid);
		CatalogManager::registerIndexInfo(t, u, pkid, pk);
	});
	using namespace debug::strutil;
	debug::print::ln("Query OK", "("_s + sec + " sec)");
}

void API::dropTable(
	const std::string &table
)
{
	debug::print::ln("drop table", table);
	auto sec = debug::time([&]{
		auto table_id = CatalogManager::getTableId(table);
		CatalogManager::removeTableInfo(table);
	});
	using namespace debug::strutil;
	debug::print::ln("Query OK", "("_s + sec + " sec)");
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
	auto sec = debug::time([&]
	{
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		ItemValue insertVal;
		if (values.size() != type.size())
		{
			throw InterpretError("number of value mismatches.");
		}
		for (int i = 0; i != values.size(); ++i)
		{
			switch ((type[i] & 0xff0000) >> 16)
			{
				case 0x10: {		// SQL_INT
					insertVal.emplace_back(int(values[i]->toNumber()));
				} break;
				case 0x40: {		// SQL_FLOAT
					insertVal.emplace_back(float(values[i]->toNumber()));
				} break;
				case 0x20: {		// SQL_CHAR
					insertVal.emplace_back(std::string(values[i]->toString()));
				} break;
				case 0x80: {
					throw InterpretError("internal error.");
				} break;
			}
		}
		IndexManager::insertData(table_id, insertVal);
	});
	using namespace debug::strutil;
	debug::print::ln("Query OK", "("_s + sec + " sec)");
}

void API::showTables()
{
	int n;
	auto sec = debug::time([&]
	{
		Sheet s;
		auto &v = s.createRow();
		v.emplace_back("tables_of_minisql");
		for (auto &str: CatalogManager::getTableNames())
		{
			s.createRow().emplace_back(str);
		}
		n = s.size() - 1;
		std::cout << s << std::endl;
	});
	using namespace debug::strutil;
	debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
}


void API::showIndexs(const std::string &tableName)
{
	int n;
	auto sec = debug::time([&]{
		Sheet s;
		s.createRow().emplace_back("index_of_" + tableName);
		for (auto &e: CatalogManager::getIndexProperties(tableName))
		{
			s.createRow().emplace_back(e.name);
		}
		n = s.size() - 1;
		std::cout << s << std::endl;
	});
	using namespace debug::strutil;
	debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
}

}