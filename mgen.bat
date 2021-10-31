cd lib/modules 
cppgen.py
cd ../..

clang-format lib/modules/*.cpp -i
clang-format lib/modules/*.h -i