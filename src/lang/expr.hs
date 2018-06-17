#define eval(x) \
	(std::move(std::get<Expr>(ast[x].gen())))

"expr"_p =
	"expr"_p + "and"_t + "expr6"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "&&", eval(1));
		})
	|"expr"_p + "&&"_t + "expr6"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "&&", eval(1));
		})
	|"expr6"_p
		>> Pass(),
"expr6"_p = 
	"expr6"_p + "="_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr(eval(0), "==", eval(1));
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
	"-"_t + "expr7"_p
		>> reflect([](AstType &ast) -> ValueType {
			return createExpr("-", eval(0));
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
		}),