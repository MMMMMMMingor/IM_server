###
 # @Author: Firefly
 # @Date: 2020-03-30 15:06:50
 # @Descripttion: 
 # @LastEditTime: 2020-03-30 15:18:27
 ###
#!/usr/bin/env bash

mkdir src/message
protoc --proto_path=proto/  --cpp_out=src/message/ proto/*.proto
mkdir build
cd build || exit
cmake ..
make
# make test