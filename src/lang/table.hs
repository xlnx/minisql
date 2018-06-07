"table_references"_p = 
	"table_references"_p + ","_t + "escaped_table_reference"_p
		>> reflect([](AstType &ast) -> ValueType {
			throw InterpretError("'join' operation is not supported.");
		})
	|"escaped_table_reference"_p
		>> Pass(),
"escaped_table_reference"_p =
	"table_reference"_p
		>> Pass(),
"table_reference"_p =
	"table_factor"_p
		>> Pass()
	|"join_table"_p
		>> reflect([](AstType &ast) -> ValueType {
			throw InterpretError("'join' operation is not supported.");
		}),
"table_factor"_p =
	"id"_t + "alias_any"_p 
		>> reflect([](AstType &ast) -> ValueType {
			return std::get<std::string>(ast.term(0));
		})
	|"("_t + "query"_p + ")"_t + "alias"_p
		>> reflect([](AstType &ast) -> ValueType {
			throw InterpretError("subquery is not supported.");
		})
	|"("_t + "table_references"_p + ")"_t
		>> Pass(),
