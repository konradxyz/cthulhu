#!/bin/bash
<<<<<<< HEAD
set -eax
mkdir -p bin
mkdir -p gen
cd gen
bnfc -m -cpp_stl -p parser ../parser/cthulhu.cf
=======
set -ax
mkdir -p gen
cd gen
bnfc -cpp_stl -p parser ../analyzer/cthulhu.cf

make
rm *.C
rm *.cpp
cd ..
>>>>>>> memory
