"select_expr"_p =
	"expr"_p
		>> Expand()
	|"expr"_p + "as"_t + "id"_t 
		>> Expand(),
"select_exprs"_p =
	"select_expr"_p
		>> Expand()
	|"select_expr"_p + ","_t + "select_exprs"_p
		>> Expand(),
"select_exprs_unqualified"_p = 
	"*"_t
		>> Expand()
	|"select_exprs"_p
		>> Expand(),

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
		>> Expand()
	|""_t
		>> NoReflect(),
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
	"where_clause_any"_p + 
	// "having_clause_any"_p + 
	"order_clause_any"_p
	// "into_clause_any"_p
		>> Expand(),

"inst_select"_p =
	"select"_t + "select_exprs_unqualified"_p
		>> Expand()
	|"select"_t + "select_exprs_unqualified"_p + "from"_t + "table_references"_p
		>> Expand()
	|"select"_t + "select_exprs_unqualified"_p + "from"_t + "table_references"_p + "select_conditions"_p
		>> Expand(),