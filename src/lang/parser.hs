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

// expr
#include <lang/expr.hs>

// alias
#include <lang/alias.hs>

// table
#include <lang/table.hs>

// select
#include <lang/select.hs>

// select
#include <lang/insert.hs>

"query"_p = 
	"inst_select"_p
		>> Pass(),

"instructions"_p = 
	"instruction"_p + "instructions"_p
		>> Expand()
	|""_t
		>> NoReflect(),
"instruction_body"_p = 
	"query"_p
		>> Pass()
	|"inst_insert"_p
		>> Pass(),
"instruction"_p = 
	"instruction_body"_p + ";"_t
		>> Pass(),

"start"_p = 
	"instructions"_p
		>> Expand()