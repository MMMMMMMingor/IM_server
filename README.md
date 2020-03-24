# 文明——IM服务器
### 项目介绍

本项目是实现一个简单的聊天室，聊天室分为服务端和客户端。采用C/S模型，使用TCP连接, epoll多路复用技术。

### 使用
```shell
mkdir build
cd build
cmake ..
make
# 启动服务器
./IM_server 

# 启动客户端
./client 

```

