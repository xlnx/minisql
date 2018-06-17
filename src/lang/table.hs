"table_references"_p =
	"id"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::get<std::string>(ast.term(0));
		}),
