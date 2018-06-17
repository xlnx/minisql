"value"_p =
	"expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto v = std::move(std::get<Expr>(ast[0].gen()));
			if (auto ptr = dynamic_cast<LiteralNode*>(v.get()))
				return std::move(ptr->getValue());
			else
				throw InterpretError("cannot insert non-literal.");
		}),
"value_list"_p =
	"value_list"_p + ","_t + "expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto V = std::get<std::vector<Value>>(ast[0].gen());
			auto v = std::move(std::get<Expr>(ast[1].gen()));
			if (auto ptr = dynamic_cast<LiteralNode*>(v.get()))
				V.emplace_back(std::move(ptr->getValue()));
			else
				throw InterpretError("cannot insert non-literal.");
			return std::move(V);
		})
	|"expr"_p
		>> reflect([](AstType &ast) -> ValueType {
			std::vector<Value> V;
			auto v = std::move(std::get<Expr>(ast[0].gen()));
			if (auto ptr = dynamic_cast<LiteralNode*>(v.get()))
				V.emplace_back(std::move(ptr->getValue()));
			else
				throw InterpretError("cannot insert non-literal.");
			return std::move(V);
		}),
	
"inst_insert"_p = 
	"insert"_t + "into"_t + "id"_t + "values"_t + "("_t + "value_list"_p + ")"_t
		>> reflect([](AstType &ast) -> ValueType {
			auto table = std::get<std::string>(ast.term(2));
			auto vec = std::get<std::vector<Value>>(ast[0].gen());
			API::insert(table, vec);
			return ValueType();
		}),