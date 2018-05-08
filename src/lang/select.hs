"select_expr"_p =
	"expr"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr"_p + "as"_t + "id"_t 
		>> reflect([](AstType &ast) -> ValueType {
			return ValueType();
		}),
"select_exprs"_p =
	"select_expr"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"select_expr"_p + ","_t + "select_exprs"_p
		>> reflect([](AstType &ast) -> ValueType {
			
		}),
"select_exprs_unqualified"_p = 
	"*"_t
		>> reflect([](AstType &ast) -> ValueType {
			
		})
	|"select_exprs"_p
		>> reflect([](AstType &ast) -> ValueType {
			
		}),
"inst_select"_p =
	"select"_t + "select_exprs_unqualified"_p
		>> reflect([](AstType &ast) -> ValueType {

			return ValueType();
		})
	|"select"_t + "select_exprs_unqualified"_p + "from"_t + "table_references"_p
		>> reflect([](AstType &ast) -> ValueType {
			return ValueType();
		})
	|"select"_t + "select_exprs_unqualified"_p + "from"_t + "table_references"_p + "select_conditions"_p
		>> reflect([](AstType &ast) -> ValueType {
			return ValueType();
		}),
