"inst_index"_p = 
	"create"_t + "index"_t + "id"_t + "on"_t + "id"_t + "("_t + "id"_t + ")"_t
		>> reflect([](AstType &ast) -> ValueType {
			API::createIndex(
				std::get<std::string>(ast.term(4)),
				std::get<std::string>(ast.term(6)),
				std::get<std::string>(ast.term(2))
			);
			return ValueType();
		})
	|"drop"_t + "index"_t + "id"_t
		>> reflect([](AstType &ast) -> ValueType {
			API::dropIndex(
				std::get<std::string>(ast.term(2))
			);
			return ValueType();
		}),