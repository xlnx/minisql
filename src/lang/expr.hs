#define eval(x) \
	(std::move(std::get<Expr>(ast[x].gen())))

"expr"_p =
	"expr1"_p
		>> reflect([](AstType &ast) -> ValueType {
			auto expr = ast[0].gen();
			// std::cout << "get expr: " << std::get<Expr>(expr) << std::endl;
			return std::move(expr);
		}),
		//Pass(),
"expr1"_p = 
	"expr1"_p + "or"_t + "expr2"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "||", eval(1));
		})
	|"expr1"_p + "||"_t + "expr2"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "||", eval(1));
		})
	|"expr2"_p
		>> Pass(),
"expr2"_p = 
	"expr2"_p + "xor"_t + "expr3"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "^", eval(1));
		})
	|"expr3"_p
		>> Pass(),
"expr3"_p = 
	"expr3"_p + "and"_t + "expr4"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "&&", eval(1));
		})
	|"expr3"_p + "&&"_t + "expr4"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "&&", eval(1));
		})
	|"expr4"_p
		>> Pass(),
"expr4"_p = 
	"not"_t + "expr4"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr("!", eval(0));
		})
	|"expr5"_p
		>> Pass(),
"expr5"_p = 
	"expr5"_p + ""_t + "expr6"_p
		>> reflect([](AstType &ast) -> ValueType {
			return ValueType();
		})
	|"expr6"_p
		>> Pass(),
"expr6"_p = 
	"expr6"_p + "="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "==", eval(1));
		})
	|"expr6"_p + "<=>"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "<=>", eval(1));
		})
	|"expr6"_p + ">="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), ">=", eval(1));
		})
	|"expr6"_p + ">"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), ">", eval(1));
		})
	|"expr6"_p + "<="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "<=", eval(1));
		})
	|"expr6"_p + "<"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "<", eval(1));
		})
	|"expr6"_p + "<>"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "!=", eval(1));
		})
	|"expr6"_p + "!="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "!=", eval(1));
		})
	|"expr7"_p
		>> Pass(),
"expr7"_p = 
	"expr7"_p + "|"_t + "expr8"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "|", eval(1));
		})
	|"expr8"_p
		>> Pass(),
"expr8"_p = 
	"expr8"_p + "&"_t + "expr9"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "&", eval(1));
		})
	|"expr9"_p
		>> Pass(),
"expr9"_p = 
	"expr9"_p + "<<"_t + "expra"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "<<", eval(1));
		})
	|"expr9"_p + ">>"_t + "expra"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), ">>", eval(1));
		})
	|"expra"_p
		>> Pass(),
"expra"_p = 
	"expra"_p + "-"_t + "exprb"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "-", eval(1));
		})
	|"expra"_p + "+"_t + "exprb"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "+", eval(1));
		})
	|"exprb"_p
		>> Pass(),
"exprb"_p = 
	"exprb"_p + "*"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "*", eval(1));
		})
	|"exprb"_p + "/"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "/", eval(1));
		})
	|"exprb"_p + "div"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "/", eval(1));
		})
	|"exprb"_p + "%"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "%", eval(1));
		})
	|"exprb"_p + "mod"_t + "exprc"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "%", eval(1));
		})
	|"exprc"_p
		>> Pass(),
"exprc"_p = 
	"exprc"_p + "^"_t + "exprd"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "^", eval(1));
		})
	|"exprd"_p
		>> Pass(),
"exprd"_p = 
	"-"_t + "exprd"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr("-", eval(0));
		})
	|"~"_t + "exprd"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr("~", eval(0));
		})
	|"expre"_p
		>> Pass(),
"expre"_p = 
	"!"_t + "expre"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr("!", eval(0));
		})
	|"exprf"_p
		>> Pass(),
"exprf"_p = 
	"number"_t
		>> reflect([](AstType &ast) -> ValueType {
			return createLiteral(std::get<double>(ast.term(0)));
		})
	|"string"_t
		>> reflect([](AstType &ast) -> ValueType {
			return createLiteral(std::get<std::string>(ast.term(0)));
		})
	|"null"_t 
		>> reflect([](AstType &ast) -> ValueType {
			return createLiteral();
		})
	|"true"_t
		>> reflect([](AstType &ast) -> ValueType {
			return createLiteral(true);
		})
	|"false"_t
		>> reflect([](AstType &ast) -> ValueType {
			return createLiteral(false);
		})
	|"id"_t
		>> reflect([](AstType &ast) -> ValueType {
			return createColumn(std::get<std::string>(ast.term(0)));
		})
	|"("_t + "expr"_p + ")"_t
		>> Pass(),
"not|bool|null"_p = 
	"true"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::make_pair(false, IsTrue);
		})
	|"not"_t + "true"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::make_pair(true, IsTrue);
		})
	|"false"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::make_pair(false, IsFalse);
		})
	|"not"_t + "false"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::make_pair(true, IsFalse);
		})
	|"unknown"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::make_pair(false, IsUnknown);
		})
	|"not"_t + "unknown"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::make_pair(true, IsUnknown);
		})
	|"null"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::make_pair(false, IsUnknown);
		})
	|"not"_t + "null"_t
		>> reflect([](AstType &ast) -> ValueType {
			return std::make_pair(true, IsUnknown);
		}),