"alias"_p =
	"as"_t + "id"_t
		>> reflect([](AstType &ast) -> ValueType {
			throw InterpretError("'as' operation is not supported.");
		}),
"alias_any"_p =
	"alias"_p
		>> Pass()
	|""_t
		>> NoReflect(),