# 文明——IM服务器
### 项目介绍

本项目是实现一个简单的聊天室，聊天室分为服务端和客户端。
使用TCP、 epoll多路复用技术。

使用 dispatcher + handler 模式对事件进行分发处理。

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

