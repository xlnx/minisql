#define Expand() \
reflect([](AstType &ast){\
	for (auto &e: ast) e.gen(); return ValueType();\
})
#define NoReflect() \
reflect([](AstType &ast){\
	return ValueType();\
})
#define Pass() \
reflect([](AstType &ast){\
	return ValueType(ast[0].gen());\
})

"start"_p = 
	"instructions"_p
		>> Expand(),
"instructions"_p = 
	"instruction"_p + "instructions"_p
		>> Expand()
	|""_t
		>> NoReflect(),
"instruction_body"_p = 
	"inst_select"_p
		>> Pass(),
"instruction"_p = 
	"instruction_body"_p + ";"_t
		>> Pass(),

// expr
#include <lang/expr.hs>

// select
"select_expr"_p =
	"col_expr"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"col_expr"_p + "as"_t + "id"_t 
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
"select_exprs_any"_p = 
	"*"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"select_exprs"_p
		>> reflect([](AstType &ast) -> ValueType {

		}),
"inst_select"_p =
	"select"_t + "select_exprs"_p
		>> reflect([](AstType &ast) -> ValueType {

			return ValueType();
		}),
	// |"select"_t + "select_exprs"_p + "from"_t + 