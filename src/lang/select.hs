"select_expr"_p =
	"expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto elem = std::move(std::get<Expr>(ast[0].gen()));
			if (auto ptr = dynamic_cast<ColumnNode*>(elem.get()))
				return ptr->getName();
			else
				throw InterpretError("expression in selection column is not supported.");
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

"inst_select"_p =
	"select"_t + "select_exprs_unqualified"_p + "from"_t + "id"_t
		>> reflect([](AstType &ast) -> ValueType {
			auto select_exprs = ast[0].gen();
			auto table = std::get<std::string>(ast.term(2));
			if (std::holds_alternative<Flag>(select_exprs))
				API::select(table);
			else
				API::select(table, std::get<std::vector<std::string>>(select_exprs));
			return ValueType();
		})
	|"select"_t + "select_exprs_unqualified"_p + "from"_t + "id"_t + "where"_t + "expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto select_exprs = ast[0].gen();
			auto table = std::get<std::string>(ast.term(2));
			auto cond = std::move(std::get<Expr>(ast[1].gen()));
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