# minisql

```sql
>>> select 1;
get expr: 1

>>> select * from a where 12 * 1e-2 / "12e2" - 12. + true / 1^1;
get expr: null

>>> select u, v from a, b where a > "123" and b < huh order by what;
get expr: u
get expr: v
get expr: a > "123" && b < huh
id

>>> select * from sheet
minisql-interpreter: error: unexpected EOF

>>> select * from "table";
minisql-interpreter:5:14: error: unexpected '"table"' due to invalid syntax
select * from "table";
              ^~~~~~~

>>> select null;
get expr: null

>>> select everything from # @$ \ .'
minisql-interpreter:6:23: error: unexpected '#' due to invalid syntax
select everything from # @$ \ .'
                       ^

>>> 12 fedsfsdr dwe rfew gre wra gafd sd
minisql-interpreter:7:0: error: unexpected '12' due to invalid syntax
12 fedsfsdr dwe rfew gre wra gafd sd
^~

```

# build

Make sure your compiler support **C++17**.