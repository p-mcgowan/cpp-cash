all: cash.cpp cash.h; g++ -std=c++11 cash.cpp -o cash 2>&1 |tee compilation.log
clean: ;rm cash
