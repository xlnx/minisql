"insert"_t = "insert"_riw,
"into"_t = "into"_riw,
"values"_t = "values"_riw, 
"order"_t = "order"_riw, 
"group"_t = "group"_riw, 
"cast"_t = "cast"_riw, 
"by"_t = "by"_riw, 
"asc"_t = "asc"_riw, 
"desc"_t = "desc"_riw, 
"ignore"_t = "ignore"_riw,
"select"_t = "select"_riw, 
"from"_t = "from"_riw, 
"where"_t = "where"_riw, 
"having"_t = "having"_riw, 
"as"_t = "as"_riw, 

"is"_t = "is"_riw, 

"true"_t = "true"_riw, 
"false"_t = "false"_riw, 
"unknown"_t = "unknown"_riw, 

"all"_t = "all"_riw, 
"any"_t = "any"_riw, 

"null"_t = "null"_riw, 

"or"_t = "or"_riw, 
"xor"_t = "xor"_riw, 
"and"_t = "and"_riw, 
"not"_t = "not"_riw, 

"<<"_t, 
">>"_t, 
"-"_t, 
"+"_t, 
"*"_t, 
"/"_t, 
"mod"_t = "mod"_riw, 
"div"_t = "div"_riw, 
"%"_t, 
"^"_t, 
"<=>"_t, 

"binary"_t = "binary"_riw, 
"row"_t = "row"_riw, 
"exists"_t = "exists"_riw, 
"&&"_t, 
"||"_t, 
"|"_t, 
"&"_t, 
","_t, 
">="_t, 
"<="_t, 
"!="_t, 
"<>"_t,
">"_t, 
"<"_t, 
"="_t, 
":="_t, 
";"_t,
"("_t,
")"_t,
","_t,
"~"_t, 
"!"_t, 

"string"_t = 
	"\"(?:[^\\\"\\\\]|\\\\.)*\""_r
		>> lexer_reflect<AstType>([](const ::std::string &src) -> ValueType {
			auto s = new char[src.length()], q = s;
			const char *p = &src[1];
			while (*p != '"')
			{
				if ((*q = *p) == '\\') *q = *++p;
				++p; ++q;
			}
			*q = 0;
			std::string ss = s;
			delete [] s;
			return ss;
		}),

"number"_t =
	"\\d*\\.\\d+|\\d+\\.\\d*|\\d+(?:[eE]-?\\d+)?"_rw
		>> lexer_reflect<AstType>([](const ::std::string &src) -> ValueType {
			::std::istringstream is(src);
			double x; is >> x; return x;
		}),

"id"_t =
	"[A-Za-z_]\\w*"_rw
		>> lexer_reflect<AstType>([](const ::std::string &src) -> ValueType {
			return std::string(src);
		}),

"stray"_t = "."_r