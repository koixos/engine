#!/bin/bash
set echo on

mkdir -p ../bin

cFileNames=$(find . -type f -name "*.c")

assembly="testbed"
compilerFlags="-g -fdeclspec -fPIC"
includeFlags="-Isrc -I../engine/src/"
linkerFlags="-L../bin/ -lengine -Wl,-rpath,."
defines="-D_DEBUG -DKIMPORT"

echo "Building $assembly..."
echo clang $cFileNames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
clang $cFileNames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags