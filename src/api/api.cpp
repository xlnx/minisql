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

void API::triv(const Expr &expr, std::map<std::string, VarRange> &varRange, bool topLevelAnd)
{		// TODO: 
	using namespace __interpret;
	if (auto unary = dynamic_cast<UnaryExprNode*>(expr.get()))
	{
		throw InterpretError("operator '" + unary->op + "' not supported.");
	}
	else if (auto binary = dynamic_cast<BinaryExprNode*>(expr.get()))
	{
		static std::string no = "|^/*+-l%";
		if (std::find(no.begin(), no.end(), binary->op[0]) != no.end() || 
			binary->op == "<<" || 
			binary->op == ">>")
		{
			throw InterpretError("operator '" + binary->op + "' not supported.");
		}
		else if (binary->op == "&&")
		{
			if (topLevelAnd)
			{
				//
			}
			else
			{
				throw InterpretError("non-conjunctive and operator not supported.");
			}
		}
		else if (binary->op[0] == '>')
		{
			if (binary->op.length() > 1)
			{

			}
			else
			{
				
			}
		}
		else if (binary->op[0] == '<')
		{
			if (binary->op.length() > 1)
			{
				
			}
			else
			{
				
			}
		}
		else
		{

		}
	}
}

BufferType API::cond2RangeFilter(
	const ItemType &itemType,
	const std::map<std::string, std::pair<BufferType, SizeType>> &attrs,
	const Expr &cond,
	std::pair<AttributeValue, AttributeValue> &range,
	std::function<bool(const Item &)> &filter
)
{
	std::map<std::string, VarRange> varRange;
	triv(cond, varRange, true);
	BufferType index_id = 0;
	return index_id;
}

void API::select(
	const std::string &tableName
)		// TODO: 
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
			auto no = e.attrno;
		}
		BufferType index_id = 0;
		std::pair<AttributeValue, AttributeValue> range;
		for (auto &e: IndexManager::queryData(index_id, range, [](Item item) -> bool { return true; }))
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
)		// TODO: 
{
	debug::print::ln("select * from", tableName, "where cond");
}

void API::select(
	const std::string &tableName, 
	const std::vector<std::string> &row
)		// TODO: 
{
	debug::print::ln("select row from", tableName);
}

void API::select(
	const std::string &tableName,
	const std::vector<std::string> &row,
	const Expr &cond
)		// TODO: 
{
	// debug::print::ln("select * from", tableName);
	int n;
	auto sec = debug::time([&]{
		Sheet s;
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		auto &newRow = s.createRow();
		auto atname = CatalogManager::getAttributeNames(tableName);
		for (auto &str: atname)
		{
			newRow.emplace_back(str);
		}
		for (auto &e: row)
		{
			if (std::find(atname.begin(), atname.end(), e) == atname.end()) 
			{
				throw InterpretError("table '" + tableName + "' has no attribute named '" + e + "'.");
			}
		}
		auto props = CatalogManager::getIndexProperties(tableName);
		std::map<std::string, std::pair<BufferType, SizeType>> attrs;
		for (auto &e: props)
		{
			attrs[e.name] = std::make_pair(e.index_id, e.attrno);
		}

		std::pair<AttributeValue, AttributeValue> range;
		std::function<bool(const Item &)> filter;
		BufferType index_id = cond2RangeFilter(type, attrs, cond, range, filter);

		for (auto &e: IndexManager::queryData(index_id, range, [](Item item) -> bool { return true; }))
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

void API::erase(
	const std::string &tableName
)		// TODO: 
{
	debug::print::ln("delete from", tableName);
}

void API::erase(
	const std::string &tableName,
	const Expr &cond
)		// TODO: 
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
		if (CatalogManager::tableExist(table))
		{
			throw InterpretError("table named '" + table + "' already exists.");
		}
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
	auto sec = debug::time([&]
	{
		auto table_id = CatalogManager::getTableId(table);
		IndexManager::dropAllIndex(table_id);
		BufferManager::removeBufferType(table_id);
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
	auto sec = debug::time([&] 
	{
		std::string pk = "";
		int pkid = -1;
		auto attrs = CatalogManager::getAttributeProperties(table);
		auto pos = -1;
		for (int i = 0; i != attrs.size(); ++i)
		{
			if (attrs[i].name == attr)
			{
				if (attrs[i].isUnique)
				{
					pos = i;
					break;
				}
				else if (attrs[i].isPrimary)
				{
					throw InterpretError("cannot create index for primary key '" + attr + "'.");
				}
				else 
				{
					throw InterpretError("attribute '" + attr + "' not ensured to be unique.");
				}
			}
		}
		if (pos == -1)
		{
			throw InterpretError("no attribute named '" + attr + "' in table '" + table + "'.");
		}
		for (auto &e: CatalogManager::getIndexProperties(table))
		{
			if (e.attrno == pos)
			{
				throw InterpretError("index named '" + e.name + "' for attribute '" + attr + "' already exists.");
			}
		}

		auto table_id = CatalogManager::getTableId(table);
		auto u = IndexManager::createIndex(table_id, pos);
		CatalogManager::registerIndexInfo(table_id, u, pos, indexName);
	});
	using namespace debug::strutil;
	debug::print::ln("Query OK", "("_s + sec + " sec)");
}

void API::dropIndex(
	const std::string &indexName
)		// TODO: 
{
	auto sec = debug::time([&]
	{
		auto index_id = CatalogManager::getIndexId(indexName);
		IndexManager::dropIndex(index_id);
		CatalogManager::removeIndexInfo(indexName);
	});
	using namespace debug::strutil;
	debug::print::ln("Query OK", "("_s + sec + " sec)");
}

void API::insert(
	const std::string &tableName,
	const std::vector<Value> &values
)
{
	bool ok; int err;
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
		ok = IndexManager::insertData(table_id, insertVal, err);
	});
	using namespace debug::strutil;
	if (ok)
	{
		debug::print::ln("Query OK", "("_s + sec + " sec)");
	}
	else
	{
		auto attrs = CatalogManager::getAttributeNames(tableName);
		debug::print::ln(std::string("Error: duplicate entry for index '") + attrs[err] + "'.");
	}
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