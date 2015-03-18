#!/bin/bash

set -eax
mkdir -p gen
cd gen
bnfc -cpp_stl -p parser ../analyzer/cthulhu.cf

make
#rm *.C
#rm *.cpp
cd ..
