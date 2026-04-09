#!/bin/sh

printf "\ncpplint: lints\n"
cpplint --filter="-build/c++11, -build/include_subdir" --exclude="inst/include/pathomove.h" inst/include/*.h

printf "\ncppcheck: issues\n"
cppcheck --std=c++14 --enable=performance,portability,warning,style --inline-suppr --error-exitcode=1 src/*.cpp
cppcheck --std=c++14 --language=c++ --enable=performance,portability,warning,style --inline-suppr --error-exitcode=1 inst/include/*.h
