#!/usr/bin/env bash
if [ ! -d "/data/" ];then
mkdir src/message
fi
protoc --proto_path=proto/  --cpp_out=src/message/ proto/*.proto
mkdir build
cd build || exit
cmake ..
make
# make test