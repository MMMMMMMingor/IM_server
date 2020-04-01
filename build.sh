###
 # @Author: Firefly
 # @Date: 2020-03-31 21:42:07
 # @Descripttion: 
 # @LastEditTime: 2020-03-31 21:44:16
 ###
#!/usr/bin/env bash

mkdir src/message
protoc --proto_path=proto/  --cpp_out=src/message/ proto/*.proto
mkdir build
cd build || exit
cmake ..
make
# make test