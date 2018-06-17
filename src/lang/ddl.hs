"create_type"_p = 
	"int"_t 
		>> reflect([](AstType &ast) -> ValueType {
			return SQL_INT;
		})
	|"float"_t
		>> reflect([](AstType &ast) -> ValueType {
			return SQL_FLOAT;
		})
	|"char"_t + "("_t + "number"_t + ")"_t 
		>> reflect([](AstType &ast) -> ValueType {
			auto n = std::get<double>(ast.term(2));
			unsigned N = n;
			if (N != n)
				throw InterpretError("char array length cannot be floating-point.");
			if (N < 1 || N > 255)
				throw InterpretError("char array length must fall in range [1, 255].");
			return SQL_CHAR(N);
		}),
"create_extra_qualifier"_p =
	"primary"_t + "key"_t + "("_t + "id"_t + ")"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::move(ast.term(3));
		}),
"create_qualifier"_p =
	"unique"_t
		>> reflect([](AstType &ast) -> ValueType {
			TableAttribute q; q.isUnique = true;
			return q;
		})
	|"primary"_t + "key"_t
		>> reflect([](AstType &ast) -> ValueType {
			TableAttribute q; q.isPrimary = true;
			return q;
		})
	|""_t
		>> reflect([](AstType &ast) -> ValueType {
			return TableAttribute();
		}),
"create_content"_p =
	"id"_t + "create_type"_p + "create_qualifier"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto Q = std::get<TableAttribute>(ast[1].gen());
			Q.type = std::get<BufferElem>(ast[0].gen());
			Q.name = std::get<std::string>(ast.term(0));
			return Q;
		}),
"create_contents"_p =
	"create_contents_aux"_p + ","_t + "create_extra_qualifier"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto M = std::get<std::vector<TableAttribute>>(ast[0].gen());
			auto k = std::get<std::string>(ast[1].gen());
			for (auto &m: M) if (m.name == k) 
			{
				m.isPrimary = true; return M;
			}
			throw InterpretError("undefined symbol '" + k + "'.");
		})
	|"create_contents_aux"_p
		>> Pass(),
"create_contents_aux"_p = 
	"create_contents_aux"_p + ","_t +  "create_content"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto M = std::get<std::vector<TableAttribute>>(ast[0].gen());
			M.emplace_back(std::get<TableAttribute>(ast[1].gen()));
			return M;
		})
	|"create_content"_p
		>> reflect([](AstType &ast) -> ValueType {
			std::vector<TableAttribute> M;
			M.emplace_back(std::get<TableAttribute>(ast[0].gen()));
			return M;
		}),
"inst_ddl"_p = 
	"create"_t + "table"_t + "id"_t + "("_t + "create_contents"_p + ")"_t 
		>> reflect([](AstType &ast) -> ValueType {
			API::createTable(std::get<std::string>(ast.term(2)), 
				std::get<std::vector<TableAttribute>>(ast[0].gen()));
			return ValueType();
		})
	|"drop"_t + "table"_t + "id"_t
		>> reflect([](AstType &ast) -> ValueType {
			API::dropTable(std::get<std::string>(ast.term(2)));
			return ValueType();
		}),