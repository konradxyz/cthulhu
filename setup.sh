#!/bin/bash
set -eax
mkdir -p bin
mkdir -p gen
cd gen
bnfc -m -cpp_stl -p parser ../parser/cthulhu.cf
