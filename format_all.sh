find bin/ -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
find lib/ -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i