cd ./build
gprof.exe ./minisql.exe | gprof2dot.exe | dot -Tpng -o __profile.png
cd ..
code ./build/__profile.png
