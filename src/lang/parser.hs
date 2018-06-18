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

// select
#include <lang/select.hs>

// select
#include <lang/insert.hs>

// index
#include <lang/index.hs>

// ddl
#include <lang/ddl.hs>

"instructions"_p = 
	"instructions"_p + "inst_select"_p + ";"_t
		>> Expand()
	|"instructions"_p + "insert"_t + "into"_t + "id"_t + "values"_t + "("_t + "value_list"_p + ")"_t + ";"_t
		>> reflect([](AstType &ast) -> ValueType {
			ast[0].gen();
			API::insert(std::get<std::string>(ast.term(2)), 
				std::get<std::vector<Value>>(ast[1].gen()));
			return ValueType();
		})
	|"instructions"_p + "inst_ddl"_p + ";"_t
		>> Expand()
	|"instructions"_p + "inst_index"_p + ";"_t
		>> Expand()
	|"instructions"_p + "inst_delete"_p + ";"_t
		>> Expand()
	|""_t
		>> NoReflect(),

"start"_p = 
	"instructions"_p
		>> Expand()
	|"quit"_t + ";"_t
		>> reflect([](AstType &ast) -> ValueType {
			std::cout << "See you next time! >w<" << std::endl;
			exit(0); return ValueType();
		})
	|"quit"_t
		>> reflect([](AstType &ast) -> ValueType {
			std::cout << "See you next time! >w<" << std::endl;
			exit(0); return ValueType();
		})
	|"execfile"_t + "string"_t + ";"_t
		>> reflect([](AstType &ast) -> ValueType {
			throw std::get<std::string>(ast.term(1));
			return ValueType();
		})
	|"show"_t + "tables"_t + ";"_t
		>> reflect([](AstType &ast) -> ValueType {
			API::showTables();
			return ValueType();
		})
	|"show"_t + "indexes"_t + "id"_t + ";"_t
		>> reflect([](AstType &ast) -> ValueType {
			API::showIndexs(std::get<std::string>(ast.term(2)));
			return ValueType();
		})