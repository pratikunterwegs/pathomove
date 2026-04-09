#!/bin/sh

find inst/include -name "*.h" ! -name "pathomove.cpp" -exec clang-format -style=google -i {} \;
clang-format -style=google -i src/pathomove.cpp
