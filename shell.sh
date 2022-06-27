#!/usr/bin/sh

if [ ! -d build/ ]; then
  mkdir build/;
fi
cd build/
export CC=gcc  

# i used ninja as builld system
cmake -G Ninja ..

ninja

#**************for make build system

# cmake -G "MinGW Makefiles" . 
#or cmake .. -DCMAKE_C_COMPILER=gcc
# make

