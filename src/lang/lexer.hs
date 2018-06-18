"string"_t = 
	"\"(?:[^\\\"\\\\]|\\\\.)*\""_r
		>> lexer_reflect<AstType>([](const ::std::string &src) -> ValueType {
			return src;
		}),

"number"_t =
	"\\d*\\.\\d+|\\d+\\.\\d*|\\d+(?:[eE]-?\\d+)?"_rw
		>> lexer_reflect<AstType>([](const ::std::string &src) -> ValueType {
			::std::istringstream is(src); double x = 0; is >> x; return x;
		}),

"insert"_t = "insert"_riw,
"into"_t = "into"_riw,
"values"_t = "values"_riw, 
"select"_t = "select"_riw, 
"from"_t = "from"_riw, 
"where"_t = "where"_riw, 

"and"_t = "and"_riw, 

"quit"_t = "quit"_riw,
"execfile"_t = "execfile"_riw,
"create"_t = "create"_riw,
"table"_t = "table"_riw,
"unique"_t = "unique"_riw,
"primary"_t = "primary"_riw,
"key"_t = "key"_riw,
"drop"_t = "drop"_riw,
"delete"_t = "delete"_riw,
"index"_t = "index"_riw,
"on"_t = "on"_riw,
"show"_t = "show"_riw,
"tables"_t = "tables"_riw,
"indexes"_t = "indexes"_riw,

"int"_t = "int"_riw,
"float"_t = "float"_riw,
"char"_t = "char"_riw,

"id"_t =
	"[A-Za-z_]\\w*"_rw
		>> lexer_reflect<AstType>([](const ::std::string &src) -> ValueType {
			return std::string(src);
		}),

"-"_t, 
"*"_t, 
"&&"_t, 
","_t, 
">="_t, 
"<="_t, 
"!="_t, 
"<>"_t,
">"_t, 
"<"_t, 
"="_t, 
";"_t,
"("_t,
")"_t,

"stray"_t = "."_r