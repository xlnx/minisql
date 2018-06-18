#include <api/api.h>
#include <interpreter/interpreter_aux.h>
#include <iomanip>
#include <climits>
#include <cfloat>

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

	void emitData(const std::vector<Item> &data, const ItemType &type, const std::vector<int> &selpos)
	{
		for (auto &e: data)
		{
			auto &row = createRow();
			auto val = BufferManager::readItem(e);
			for (int i = 0; i != selpos.size(); ++i)
			{
				auto j = selpos[i];
				switch ((type[j] & 0xff0000) >> 16)
				{
					using namespace debug::strutil;
					case 0x10: {		// SQL_INT
						row.emplace_back(""_s + std::get<int>(val[j]));
					} break;
					case 0x20: {
						row.emplace_back(std::get<std::string>(val[j]));
					} break;
					case 0x40: {
						row.emplace_back(""_s + std::get<float>(val[j]));
					} break;
				}
			}
		}
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

void API::triv(const Expr &expr, std::map<std::string, VarRange> &varRange, bool &eval)
{		// TODO: 
	using namespace __interpret;
	if (auto unary = dynamic_cast<UnaryExprNode*>(expr.get()))
	{
		throw InterpretError("operator '" + unary->op + "' not supported.");
	}
	else if (auto binary = dynamic_cast<BinaryExprNode*>(expr.get()))
	{
		auto op = binary->op;
		static std::string no = "|^/*+-l%";
		if (std::find(no.begin(), no.end(), op[0]) != no.end() || op == "<<" || op == ">>" || op == "&")
		{
			throw InterpretError("operator '" + op + "' not supported.");
		}
		else if (op == "&&")
		{
			if (eval)
			{
				triv(binary->lhs, varRange, eval);
				triv(binary->rhs, varRange, eval);
			}
		}
		else
		{
			auto l = dynamic_cast<ColumnNode*>(binary->lhs.get());
			auto r = dynamic_cast<ColumnNode*>(binary->rhs.get());

			auto u = dynamic_cast<LiteralNode*>(binary->lhs.get());
			auto v = dynamic_cast<LiteralNode*>(binary->rhs.get());

			if (l && r)
			{
				throw InterpretError("operation between two attributes not allowed.");
			}
			else if (!l && !u || !r && !v)
			{
				throw InterpretError("non-conjunctive expression not allowed.");
			}
			else if (r || u) 
			{
				std::swap(l, r); std::swap(u, v);
				switch (op[0])
				{
					case '>': op[0] = '<'; break;
					case '<': op[0] = '>'; break;
				}
			}
			switch (op[0])
			{
				case '=': {
					varRange[l->name].eq.emplace_back(&v->value);
				} break;
				case '!': {
					varRange[l->name].neq.emplace_back(&v->value);
				} break;
				case '>': {
					if (op.length() == 1) 
					{
						varRange[l->name].neq.emplace_back(&v->value);
					}
					if (!varRange[l->name].range.first || *varRange[l->name].range.first < v->value)
					{
						varRange[l->name].range.first = &v->value;
					}
				} break;
				case '<': {
					if (op.length() == 1) 
					{
						varRange[l->name].neq.emplace_back(&v->value);
					}
					if (!varRange[l->name].range.second || *varRange[l->name].range.second > v->value)
					{
						varRange[l->name].range.second = &v->value;
					}
				} break;
			}
		}
	}
	else if (auto val = dynamic_cast<LiteralNode*>(expr.get()))
	{
		eval = eval && val->value->toBool();
	}
}

BufferType API::cond2RangeFilter(
	const ItemType &itemType,
	std::map<std::string, std::pair<BufferType, SizeType>> &indexes,
	const std::vector<std::string> &attrs,
	const Expr &cond,
	bool &eval,
	std::pair<AttributeValue, AttributeValue> &range,
	std::function<bool(const Item &)> &filter
)
{
	std::map<std::string, VarRange> varRange;
	eval = true;
	triv(cond, varRange, eval);
	for (auto &e: varRange)
	{
		if (std::find(attrs.begin(), attrs.end(), e.first) == attrs.end())
		{
			throw InterpretError("attribute '" + e.first + "' does not exist.");
		}
	}
	std::map<std::string, int> attrno;
	for (int i = 0; i != attrs.size(); ++i)
	{
		attrno[attrs[i]] = i;
	}
	BufferType index_id = 0;
	if (eval)
	{
		std::string f[3];
		for (auto &e: varRange)
		{
			if (indexes.count(e.first))
			{
				switch ((itemType[attrno[e.first]] & 0xff0000) >> 16)
				{
					case 0x10: {
						f[0] = e.first;
					} break;
					case 0x40: {
						f[1] = e.first;
					} break;
					case 0x20: {
						f[2] = e.first;
					} break;
				}
			}
		}
		std::string at = f[0];
		if (at == "") at = f[1];
		if (at == "") at = f[2];
		if (at == "") at = indexes.begin()->first;
		index_id = indexes[at].first;
		auto aid = indexes[at].second;
		switch ((itemType[aid] & 0xff0000) >> 16)
		{
			case 0x10: {		// int
				range.first = INT_MIN;
				range.second = INT_MAX;
				if (varRange.count(at))
				{
					if (varRange[at].range.first)
					{
						range.first = int((*varRange[at].range.first)->toNumber());
					}
					if (varRange[at].range.second)
					{
						range.second = int((*varRange[at].range.second)->toNumber());
					}
				}
			} break;
			case 0x40: {
				range.first = FLT_MIN;
				range.second = FLT_MAX;
				if (varRange.count(at))
				{
					if (varRange[at].range.first)
					{
						range.first = float((*varRange[at].range.first)->toNumber());
					}
					if (varRange[at].range.second)
					{
						range.second = float((*varRange[at].range.second)->toNumber());
					}
				}
			}
			case 0x20: {
				range.first = std::string("");
				range.second = std::string(260, '\x7f');
				if (varRange.count(at))
				{
					if (varRange[at].range.first)
					{
						range.first = (*varRange[at].range.first)->toString();
					}
					if (varRange[at].range.second)
					{
						range.second = (*varRange[at].range.second)->toString();
					}
				}
			} break;
		}
		struct ImplRange
		{
			std::vector<AttributeValue> eq;
			std::vector<AttributeValue> neq;
			std::pair<AttributeValue, AttributeValue> range;
			std::pair<bool, bool> have = std::make_pair(false, false);
		};
		int n = attrs.size();
		std::vector<ImplRange> r(n);
		for (auto &e: varRange)
		{
			auto aid = attrno[e.first];
			auto &r0 = r[aid];
			switch ((itemType[aid] & 0xff0000) >> 16)
			{
				case 0x10: {
					for (auto &f: e.second.eq)
					{
						r[aid].eq.emplace_back(int((*f)->toNumber()));
					}
					for (auto &f: e.second.neq)
					{
						r[aid].neq.emplace_back(int((*f)->toNumber()));
					}
					if (r[aid].have.first = e.second.range.first)
					{
						r[aid].range.first = int((*e.second.range.first)->toNumber());
					}
					if (r[aid].have.second = e.second.range.second)
					{
						r[aid].range.second = int((*e.second.range.second)->toNumber());
					}
				} break;
				case 0x20: {
					for (auto &f: e.second.eq)
					{
						r[aid].eq.emplace_back((*f)->toString());
					}
					for (auto &f: e.second.neq)
					{
						r[aid].neq.emplace_back((*f)->toString());
					}
					if (r[aid].have.first = e.second.range.first)
					{
						r[aid].range.first = (*e.second.range.first)->toString();
					}
					if (r[aid].have.second = e.second.range.second)
					{
						r[aid].range.second = (*e.second.range.second)->toString();
					}
				} break;
				case 0x40: {
					for (auto &f: e.second.eq)
					{
						r[aid].eq.emplace_back(float((*f)->toNumber()));
					}
					for (auto &f: e.second.neq)
					{
						r[aid].neq.emplace_back(float((*f)->toNumber()));
					}
					if (r[aid].have.first = e.second.range.first)
					{
						r[aid].range.first = float((*e.second.range.first)->toNumber());
					}
					if (r[aid].have.second = e.second.range.second)
					{
						r[aid].range.second = float((*e.second.range.second)->toNumber());
					}
				} break;
			}
		}
		filter = [=](const Item &item) -> bool
		{
			for (int i = 0; i != n; ++i)
			{
				auto attr = item[i];
				for (auto &e: r[i].eq)
				{
					if (attr.val() != e)
					{
						return false;
					}
				}
				for (auto &e: r[i].neq)
				{
					if (attr.val() == e)
					{
						return false;
					}
				}
				if (r[i].have.first && attr.val() < r[i].range.first)
				{
					return false;
				}
				if (r[i].have.second && attr.val() > r[i].range.second)
				{
					return false;
				}
			}
			return true;
		};
	}
	return index_id;
}

void API::select(
	const std::string &tableName
)		// TODO: 
{
		// debug::print::ln("select * from", tableName);
	int n;
	auto sec = debug::time([&]{
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		
		Sheet s;
		auto &newRow = s.createRow();
		auto atname = CatalogManager::getAttributeNames(tableName);
		std::vector<int> selpos;
		for (auto i = 0; i != atname.size(); ++i)
		{
			newRow.emplace_back(atname[i]);
			selpos.emplace_back(i);
		}

		int index_rank = 3;
		BufferType index_id = -1;
		auto props = CatalogManager::getIndexProperties(tableName);
		for (auto &e: props)
		{
			switch ((type[e.attrno] & 0xff0000) >> 16)
			{
				case 0x10: {		// SQL_INT
					if (index_id)
					{
						index_id = e.index_id;
						index_rank = 1;
					}
				} break;
				case 0x40: {		// SQL_FLOAT
					if (index_rank >= 2)
					{
						index_id = e.index_id;
						index_rank = 2;
					}
				} break;
				case 0x20: {		// SQL_CHAR
					if (index_rank >= 3)
					{
						index_id = e.index_id;
						index_rank = 3;
					}
				} break;
			}
		}

		std::pair<AttributeValue, AttributeValue> range;
		switch (index_rank)
		{
			case 1: {
				range = std::make_pair<AttributeValue, AttributeValue>(INT_MIN, INT_MAX);
			} break;
			case 2: {
				range = std::make_pair<AttributeValue, AttributeValue>(FLT_MIN, FLT_MAX);
			} break;
			case 3: {
				range = std::make_pair<AttributeValue, AttributeValue>("", std::string(260, '\x7f'));
			} break;
		}

		s.emitData(IndexManager::queryData(index_id, range, [](const Item &){ return true; }), type, selpos);
		n = s.size() - 1;
		std::cout << s << std::endl;
	});
	using namespace debug::strutil;
	if (doPrint) debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
}

void API::select(
	const std::string &tableName, 
	const Expr &cond
)		// TODO: 
{
	// debug::print::ln("select * from", tableName);
	int n;
	auto sec = debug::time([&]{
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		
		Sheet s;
		auto &newRow = s.createRow();
		auto atname = CatalogManager::getAttributeNames(tableName);
		std::vector<int> selpos;
		for (auto i = 0; i != atname.size(); ++i)
		{
			newRow.emplace_back(atname[i]);
			selpos.emplace_back(i);
		}

		auto props = CatalogManager::getIndexProperties(tableName);
		std::map<std::string, std::pair<BufferType, SizeType>> indexes;
		for (auto &e: props)
		{
			indexes[e.name] = std::make_pair(e.index_id, e.attrno);
		}

		bool eval;
		std::pair<AttributeValue, AttributeValue> range;
		std::function<bool(const Item &)> filter;
		BufferType index_id = cond2RangeFilter(type, indexes, atname, cond, eval, range, filter);

		if (eval) s.emitData(IndexManager::queryData(index_id, range, filter), type, selpos);
		n = s.size() - 1;
		std::cout << s << std::endl;
	});
	using namespace debug::strutil;
	if (doPrint) debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
}

void API::select(
	const std::string &tableName, 
	const std::vector<std::string> &row
)		// TODO: 
{
	int n;
	auto sec = debug::time([&]{
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		
		Sheet s;
		auto &newRow = s.createRow();
		auto atname = CatalogManager::getAttributeNames(tableName);
		std::vector<int> selpos;
		for (auto &e: row)
		{
			auto iter = std::find(atname.begin(), atname.end(), e);
			if (iter == atname.end()) 
			{
				throw InterpretError("table '" + tableName + "' has no attribute named '" + e + "'.");
			}
			else
			{
				newRow.emplace_back(e); selpos.emplace_back(iter - atname.begin());
			}
		}

		int index_rank = 3;
		BufferType index_id = -1;
		auto props = CatalogManager::getIndexProperties(tableName);
		for (auto &e: props)
		{
			switch ((type[e.attrno] & 0xff0000) >> 16)
			{
				case 0x10: {		// SQL_INT
					if (index_id)
					{
						index_id = e.index_id;
						index_rank = 1;
					}
				} break;
				case 0x40: {		// SQL_FLOAT
					if (index_rank >= 2)
					{
						index_id = e.index_id;
						index_rank = 2;
					}
				} break;
				case 0x20: {		// SQL_CHAR
					if (index_rank >= 3)
					{
						index_id = e.index_id;
						index_rank = 3;
					}
				} break;
			}
		}

		std::pair<AttributeValue, AttributeValue> range;
		switch (index_rank)
		{
			case 1: {
				range = std::make_pair<AttributeValue, AttributeValue>(INT_MIN, INT_MAX);
			} break;
			case 2: {
				range = std::make_pair<AttributeValue, AttributeValue>(FLT_MIN, FLT_MAX);
			} break;
			case 3: {
				range = std::make_pair<AttributeValue, AttributeValue>("", std::string(260, '\x7f'));
			} break;
		}

		s.emitData(IndexManager::queryData(index_id, range, [](const Item &){ return true; }), type, selpos);
		n = s.size() - 1;
		std::cout << s << std::endl;
	});
	using namespace debug::strutil;
	if (doPrint) debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
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
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		
		Sheet s;
		auto &newRow = s.createRow();
		auto atname = CatalogManager::getAttributeNames(tableName);
		std::vector<int> selpos;
		for (auto &e: row)
		{
			auto iter = std::find(atname.begin(), atname.end(), e);
			if (iter == atname.end()) 
			{
				throw InterpretError("table '" + tableName + "' has no attribute named '" + e + "'.");
			}
			else
			{
				newRow.emplace_back(e); selpos.emplace_back(iter - atname.begin());
			}
		}

		auto props = CatalogManager::getIndexProperties(tableName);
		std::map<std::string, std::pair<BufferType, SizeType>> indexes;
		for (auto &e: props)
		{
			indexes[e.name] = std::make_pair(e.index_id, e.attrno);
		}

		bool eval;
		std::pair<AttributeValue, AttributeValue> range;
		std::function<bool(const Item &)> filter;
		BufferType index_id = cond2RangeFilter(type, indexes, atname, cond, eval, range, filter);

		if (eval) s.emitData(IndexManager::queryData(index_id, range, filter), type, selpos);
		n = s.size() - 1;
		std::cout << s << std::endl;
	});
	using namespace debug::strutil;
	if (doPrint) debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
}

void API::erase(
	const std::string &tableName
)		// TODO: 
{
	int n;
	auto sec = debug::time([&]{
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		
		auto atname = CatalogManager::getAttributeNames(tableName);

		int index_rank = 3;
		BufferType index_id = -1;
		auto props = CatalogManager::getIndexProperties(tableName);
		for (auto &e: props)
		{
			switch ((type[e.attrno] & 0xff0000) >> 16)
			{
				case 0x10: {		// SQL_INT
					if (index_id)
					{
						index_id = e.index_id;
						index_rank = 1;
					}
				} break;
				case 0x40: {		// SQL_FLOAT
					if (index_rank >= 2)
					{
						index_id = e.index_id;
						index_rank = 2;
					}
				} break;
				case 0x20: {		// SQL_CHAR
					if (index_rank >= 3)
					{
						index_id = e.index_id;
						index_rank = 3;
					}
				} break;
			}
		}

		std::pair<AttributeValue, AttributeValue> range;
		switch (index_rank)
		{
			case 1: {
				range = std::make_pair<AttributeValue, AttributeValue>(INT_MIN, INT_MAX);
			} break;
			case 2: {
				range = std::make_pair<AttributeValue, AttributeValue>(FLT_MIN, FLT_MAX);
			} break;
			case 3: {
				range = std::make_pair<AttributeValue, AttributeValue>("", std::string(260, '\x7f'));
			} break;
		}

		n = IndexManager::deleteData(index_id, range, [](const Item &){ return true; });
	});
	using namespace debug::strutil;
	if (doPrint) debug::print::ln(n, "rows affected", "("_s + sec + " sec)");
}

void API::erase(
	const std::string &tableName,
	const Expr &cond
)		// TODO: 
{
	int n;
	auto sec = debug::time([&]{
		BufferType table_id = CatalogManager::getTableId(tableName);
		auto &type = BufferManager::getItemType(table_id);
		
		auto atname = CatalogManager::getAttributeNames(tableName);

		auto props = CatalogManager::getIndexProperties(tableName);
		std::map<std::string, std::pair<BufferType, SizeType>> indexes;
		for (auto &e: props)
		{
			indexes[e.name] = std::make_pair(e.index_id, e.attrno);
		}

		bool eval;
		std::pair<AttributeValue, AttributeValue> range;
		std::function<bool(const Item &)> filter;
		BufferType index_id = cond2RangeFilter(type, indexes, atname, cond, eval, range, filter);

		if (eval)
		{
			n = IndexManager::deleteData(index_id, range, filter);
		}
		else
		{
			n = 0;
		}
	});
	using namespace debug::strutil;
	if (doPrint) debug::print::ln(n, "rows affected", "("_s + sec + " sec)");
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
	if (doPrint) debug::print::ln("Query OK", "("_s + sec + " sec)");
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
		for (auto &name: CatalogManager::getIndexNames())
		{
			CatalogManager::removeIndexInfo(name);
		}
	});
	using namespace debug::strutil;
	if (doPrint) debug::print::ln("Query OK", "("_s + sec + " sec)");
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
	if (doPrint) debug::print::ln("Query OK", "("_s + sec + " sec)");
}

void API::dropIndex(
	const std::string &indexName
)
{
	auto sec = debug::time([&]
	{
		auto index_id = CatalogManager::getIndexId(indexName);
		IndexManager::dropIndex(index_id);
		CatalogManager::removeIndexInfo(indexName);
	});
	using namespace debug::strutil;
	if (doPrint) debug::print::ln("Query OK", "("_s + sec + " sec)");
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
					auto N = type[i] & 0x00ffff;
					insertVal.emplace_back(std::string(values[i]->toString()).substr(0, N));
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
		if (doPrint) debug::print::ln("Query OK", "("_s + sec + " sec)");
	}
	else
	{
		auto attrs = CatalogManager::getAttributeNames(tableName);
		throw InterpretError("duplicate entry for index '"_s + attrs[err] + "'.");
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
	if (doPrint) debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
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
	if (doPrint) debug::print::ln(n, "rows in set", "("_s + sec + " sec)");
}

}