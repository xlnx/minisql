"number"_t =
	"(?:0[Xx][0-9A-Fa-f]+|0[0-7]*|[1-9][0-9]*)"_rw
		>> lexer_reflect<AstType>([](const ::std::string &src) -> ValueType{
			if (src[0] == '0')
			{
				if (src.length() > 1 && (src[1] == 'X' || src[1] == 'x'))
				{
					::std::istringstream is(src.substr(2));
					int x; is >> ::std::hex >> x; return x;
				}
				else
				{
					::std::istringstream is(src.substr(1));
					int x = 0; is >> ::std::oct >> x; return x;
				}
			}
			else
			{
				::std::istringstream is(src);
				int x; is >> ::std::dec >> x; return x;
			}
		}),

"select"_t, 
"from"_t, 
"where"_t, 
"as"_t, 

"is"_t, 

"true"_t, 
"false"_t, 
"unknown"_t, 

"all"_t, 
"any"_t, 

"null"_t, 

"or"_t, 
"xor"_t, 
"and"_t, 
"not"_t, 

"|"_t, 
"&"_t, 
"<<"_t, 
">>"_t, 
"-"_t, 
"+"_t, 
"*"_t, 
"/"_t, 
"mod"_t, 
"div"_t, 
"%"_t, 
"^"_t, 
"<=>"_t, 

"binary"_t, 
"row"_t, 
"exists"_t, 
"~"_t, 
"!"_t, 
"&&"_t, 
"||"_t, 
","_t, 
">"_t, 
"<"_t, 
"="_t, 
":="_t, 
">="_t, 
"<="_t, 
"!="_t, 
"<>"_t,
";"_t