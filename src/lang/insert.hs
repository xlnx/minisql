"value_list"_p =
	"value_list"_p + ","_t + "literal"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto V = std::move(std::get<std::vector<Value>>(ast[0].gen()));
			V.emplace_back(std::move(std::get<Value>(ast[1].gen())));
			return std::move(V);
		})
	|"literal"_p
		>> reflect([](AstType &ast) -> ValueType {
			std::vector<Value> V;
			V.emplace_back(std::move(std::get<Value>(ast[0].gen())));
			return std::move(V);
		}),