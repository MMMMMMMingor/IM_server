#!/usr/bin/env bash

mkdir src/message
protoc --proto_path=proto/  --cpp_out=src/message/ proto/*.proto
mkdir build
cd build || exit
cmake ..
make
# make test