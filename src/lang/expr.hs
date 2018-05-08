"expr"_p =
	"expr1"_p
		>> Pass(),
"expr1"_p = 
	"expr1"_p + "or"_t + "expr2"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto lhs = ast[0].gen().get<Expr>();
			auto rhs = ast[1].gen().get<Expr>();
			return lhs["or"](rhs);
		})
	|"expr1"_p + "||"_t + "expr2"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto lhs = ast[0].gen().get<Expr>();
			auto rhs = ast[1].gen().get<Expr>();
			return lhs["or"](rhs);
		})
	|"expr2"_p
		>> Pass(),
"expr2"_p = 
	"expr2"_p + "xor"_t + "expr3"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto lhs = ast[0].gen().get<Expr>();
			auto rhs = ast[1].gen().get<Expr>();
			return lhs["xor"](rhs);
		})
	|"expr3"_p
		>> Pass(),
"expr3"_p = 
	"expr3"_p + "and"_t + "expr4"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto lhs = ast[0].gen().get<Expr>();
			auto rhs = ast[1].gen().get<Expr>();
			return lhs["and"](rhs);
		})
	|"expr3"_p + "&&"_t + "expr4"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto lhs = ast[0].gen().get<Expr>();
			auto rhs = ast[1].gen().get<Expr>();
			return lhs["and"](rhs);
		})
	|"expr4"_p
		>> Pass(),
"expr4"_p = 
	"not"_t + "expr4"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto oper = ast[0].gen().get<Expr>();
			return oper["not"](Expr::none);
		})
	|"expr5"_p
		>> Pass(),
"expr5"_p = 
	"expr5"_p + ""_t + "expr6"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p
		>> Pass(),
"expr6"_p = 
	"expr6"_p + "="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "<=>"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + ">="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + ">"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "<="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "<"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "<>"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "!="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "is"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "like"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "regexp"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr6"_p + "in"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr7"_p
		>> Pass(),
"expr7"_p = 
	"expr7"_p + "|"_t + "expr8"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr8"_p
		>> Pass(),
"expr8"_p = 
	"expr8"_p + "&"_t + "expr9"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr9"_p
		>> Pass(),
"expr9"_p = 
	"expr9"_p + "<<"_t + "expra"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expr9"_p + ">>"_t + "expra"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expra"_p
		>> Pass(),
"expra"_p = 
	"expra"_p + "-"_t + "exprb"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expra"_p + "+"_t + "exprb"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"exprb"_p
		>> Pass(),
"exprb"_p = 
	"exprb"_p + "*"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"exprb"_p + "/"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"exprb"_p + "div"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"exprb"_p + "%"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"exprb"_p + "mod"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"exprc"_p
		>> Pass(),
"exprc"_p = 
	"exprc"_p + "^"_t + "exprd"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"exprd"_p
		>> Pass(),
"exprd"_p = 
	"-"_t + "exprd"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"~"_t + "exprd"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"expre"_p
		>> Pass(),
"expre"_p = 
	"!"_t + "expre"_p
		>> reflect([](AstType &ast) -> ValueType {

		})
	|"exprf"_p
		>> Pass(),
"exprf"_p = 
	"integer"_t
		>> reflect([](AstType &ast) -> ValueType {
			return ast.term(0).get<int>();
		})
	|"id"_t
		>> reflect([](AstType &ast) -> ValueType {
			return ast.term(0).get<std::string>();
		})
	|"("_t + "expr"_p + ")"_t
		>> Pass(),
