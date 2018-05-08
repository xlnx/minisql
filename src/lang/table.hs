"table_references"_p = 
	"escaped_table_reference"_p + ","_t + "table_references"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"escaped_table_reference"_p
		>> reflect([](AstType &ast) -> ValueType {

		}),
"escaped_table_reference"_p =
	"table_reference"_p
		>> Pass(),
"table_reference"_p =
	"table_factor"_p
		>> Pass()
	|"join_table"_p
		>> Pass(),
"table_factor"_p =
	"id"_t + "alias_any"_p 
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"("_t + "query"_p + ")"_t + "alias"_p
		>> Pass()
	|"("_t + "table_references"_p + ")"_t
		>> Pass(),
