"select_expr"_p =
	"expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto elem = std::move(std::get<Expr>(ast[0].gen()));
			if (auto ptr = dynamic_cast<ColumnNode*>(elem.get()))
				return ptr->getName();
			else
				throw InterpretError("expression in selection column is not supported.");
		})
	|"expr"_p + "as"_t + "id"_t 
		>> reflect([](AstType &ast) -> ValueType {
			throw InterpretError("'as' operation is not supported.");
		}),
"select_exprs"_p =
	"select_expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			std::vector<std::string> v; v.emplace_back(std::move(std::get<std::string>(ast[0].gen())));
			return std::move(v);
		})
	|"select_exprs"_p + ","_t + "select_expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto res = std::get<std::vector<std::string>>(ast[0].gen());
			res.emplace_back(std::move(std::get<std::string>(ast[1].gen())));
			return std::move(res);
		}),
"select_exprs_unqualified"_p = 
	"*"_t
		>> reflect([](AstType &ast) -> ValueType { return Flag(); })
	|"select_exprs"_p
		>> Pass(),

"cond"_p =
	"id"_t
		>> reflect([](AstType &ast) -> ValueType {
			std::cerr << "id" << std::endl;
			return ValueType();
		})
	|"number"_t
		>> reflect([](AstType &ast) -> ValueType {
			std::cerr << "number" << std::endl;
			return ValueType();
		})
	|"expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			std::cerr << "expr" << std::endl;
			return ast[0].gen();
		}),
"order_type"_p =
	"asc"_t
		>> NoReflect()
	|"desc"_t
		>> NoReflect(),
"order_target"_p =
	"cond"_p + "order_type_any"_p
		>> Expand(),
"order_targets"_p =
	"order_targets"_p + ","_t + "order_target"_p
		>> Expand()
	|"order_target"_p
		>> Pass(),

"order_type_any"_p =
	"order_type"_p
		>> Pass()
	|""_t
		>> NoReflect(),
"where_clause_any"_p =
	"where"_t + "expr"_p
		>> Pass()
	|""_t
		>> reflect([](AstType &ast) -> ValueType {
			throw InterpretError("lack of where condition.");
		}),
"having_clause_any"_p =
	"having"_t + "expr"_p
		>> Expand()
	|""_t
		>> NoReflect(),
"order_clause_any"_p =
	"order"_t + "by"_t + "order_targets"_p
		>> Expand()
	|""_t
		>> NoReflect(),
"group_clause_any"_p =
	"group"_t + "by"_t + "order_targets"_p
		>> Expand()
	|""_t
		>> NoReflect(),
// "into_clause_any"_p = 
// 	"into"

"select_conditions"_p = 
	"where_clause_any"_p 
	// "having_clause_any"_p + 
	// "order_clause_any"_p
	// "into_clause_any"_p
		>> Pass(),

"inst_select"_p =
	"select"_t + "select_exprs"_p
		>> reflect([](AstType &ast) -> ValueType {
			return ValueType();
		})
	|"select"_t + "select_exprs_unqualified"_p + "from"_t + "table_references"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto select_exprs = ast[0].gen();
			auto table = std::get<std::string>(ast[1].gen());
			if (std::holds_alternative<Flag>(select_exprs))
				API::select(table);
			else
				API::select(table, std::get<std::vector<std::string>>(select_exprs));
			return ValueType();
		})
	|"select"_t + "select_exprs_unqualified"_p + "from"_t + "table_references"_p + "select_conditions"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto select_exprs = ast[0].gen();
			auto table = std::get<std::string>(ast[1].gen());
			auto cond = std::move(std::get<Expr>(ast[2].gen()));
			if (std::holds_alternative<Flag>(select_exprs))
				API::select(table, cond);
			else
				API::select(table, std::get<std::vector<std::string>>(select_exprs), std::move(cond));
			return ValueType();
		}),

"inst_delete"_p = 
	"delete"_t + "from"_t + "id"_t
		>> reflect([](AstType &ast) -> ValueType {
			API::erase(std::get<std::string>(ast.term(2)));
			return ValueType();
		}) 
	|"delete"_t + "from"_t + "id"_t + "where"_t + "expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			API::erase(std::get<std::string>(ast.term(2)), 
				std::get<Expr>(ast[0].gen()));
			return ValueType();
		}),