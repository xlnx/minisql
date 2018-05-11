"value"_p =
	"expr"_p
		>> Pass(),
"insert_exprs"_p =
	"insert_exprs"_p + ","_p + "expr"_p
		>> Expand()
	|"insert_exprs"_p
		>> Pass(),
"value_list"_p =
	"value_list"_p + ","_t + "value"_p
		>> Expand()
	|"value"_p
		>> Pass(),
"insert_value_list"_p = 
	"("_t + "value_list"_p + ")"_t
		>> Expand(),
"insert_value_lists"_p =
	"insert_value_lists"_p + ","_t + "insert_value_list"_p
		>> Expand()
	|"insert_value_list"_p
		>> Pass(),
"insert_priority_any"_p =
	"asc"_t
		>> NoReflect()
	|"desc"_t
		>> NoReflect()
	|""_t
		>> NoReflect(),
"insert_attrs"_p = 
	"insert_attrs"_p + ","_t + "id"_t
		>> Expand()
	|"id"_t
		>> NoReflect(),
"insert_attrs_any"_p = 
	"("_t + "insert_attrs"_p + ")"_p
		>> Expand()
	|""_t
		>> NoReflect(),
	
"inst_insert"_p = 
	"insert"_t + "insert_priority_any"_p +
			"into"_t + "id"_t + "insert_attrs_any"_p + "values"_t +
				"insert_value_lists"_p
		>> Expand(),