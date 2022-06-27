#!/usr/bin/sh

if [ ! -d build/ ]; then
  mkdir build/;
fi
cd build/

# i used ninja as builld system
cmake -G Ninja ..

ninja

#**************for make build system

# cmake -G "MinGW Makefiles" .

