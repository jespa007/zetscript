#!/bin/bash
TARGET="Unix Makefiles"
if [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
   echo compile for mingw32 ...
   TARGET="MSYS Makefiles"
fi

cd ../../mixer
make clean
rm CMakeCache.txt
rm CMakeFiles -R
cmake CMakeLists.txt -G "$TARGET"
make -j2

# Setting up chaiscript for engine and bgd ...
cd ../engine
make clean
rm CMakeCache.txt
rm CMakeFiles -R
cmake CMakeLists.txt -G "$TARGET" -DCHAISCRIPT=yes
make -j2

cd ../bgd
make clean
rm CMakeCache.txt
rm CMakeFiles -R
cmake CMakeLists.txt -G "$TARGET" -DCHAISCRIPT=yes
make -j2
