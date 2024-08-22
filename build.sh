#!/bin/bash

cd /build

# protobuf generate
export SRC_DIR=./proto
export DST_DIR=./proto
rm -rf $DST_DIR/*.h $DST_DIR/*.cpp
protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/exchange.proto

# cmake build
rm -rf build
mkdir -p build

cd build && cmake ..
cmake --build .