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
	"value_list"_p + ","_t + "value"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto V = std::get<std::vector<Value>>(ast[0].gen());
			V.emplace_back(std::get<Value>(ast[1].gen()));
			return std::move(V);
		})
	|"value"_p
		>> reflect([](AstType &ast) -> ValueType {
			std::vector<Value> V; V.emplace_back(std::get<Value>(ast[0].gen()));
			return std::move(V);
		}),
"insert_value_list"_p = 
	"("_t + "value_list"_p + ")"_t
		>> Pass(),
"insert_value_lists"_p =
	"insert_value_lists"_p + ","_t + "insert_value_list"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto V = std::get<std::vector<std::vector<Value>>>(ast[0].gen());
			V.emplace_back(std::get<std::vector<Value>>(ast[1].gen()));
			return std::move(V);
		})
	|"insert_value_list"_p
		>> reflect([](AstType &ast) -> ValueType {
			std::vector<std::vector<Value>> V; V.emplace_back(std::get<std::vector<Value>>(ast[0].gen()));
			return std::move(V);
		}),
	
"inst_insert"_p = 
	"insert"_t + "into"_t + "id"_t + "values"_t + "insert_value_lists"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto table = std::get<std::string>(ast.term(2));
			auto vec = std::get<std::vector<std::vector<Value>>>(ast[0].gen());
			if (vec.size() != 1) 
				throw InterpretError("inserting multiple data not allowed.");
			API::insert(table, vec[0]);
			return ValueType();
		}),