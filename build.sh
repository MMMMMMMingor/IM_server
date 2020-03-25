protoc --proto_path=proto/  --cpp_out=src/message/ proto/*.proto
mkdir build
cd build
cmake ..
make
make test