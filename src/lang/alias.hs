"alias"_p =
	"as"_t + "id"_t
		>> reflect([](AstType &ast) -> ValueType {
			
		}),
"alias_any"_p =
	"alias"_p
		>> Pass()
	|""_t
		>> NoReflect(),