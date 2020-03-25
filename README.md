# 文明——IM服务器
![travis](https://travis-ci.org/MMMMMMMingor/IM_server.svg?branch=master)
### 项目介绍

本项目是实现一个简单的聊天室，聊天室分为服务端和客户端。
使用TCP、 epoll多路复用技术。

使用 dispatcher + handler 模式对事件进行分发处理。
使用 proto-buf 作为序列化协议

[proto-buf 安装教程](https://github.com/protocolbuffers/protobuf/tree/master/src)

### 使用
```shell
git clone git@github.com:MMMMMMMingor/IM_server.git
cd IM_server
mkdir build
cd build
cmake ..
make

# 启动服务器
./IM_server 

# 启动客户端
./IM_client 

```

