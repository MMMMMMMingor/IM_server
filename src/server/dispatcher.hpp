#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "common.hpp"
#include "handler.hpp"

/**
 * 事件分配器
 * @param event         事件
 * @param listen_fd     监听 socket 文件描述符
 * @param epoll_fd      epoll socket 文件描述符
 */
void dispatcher(struct epoll_event event, int listen_fd, int epoll_fd) {
  int socket_fd = event.data.fd;

  //新用户连接
  if (socket_fd == listen_fd) {
    struct sockaddr_in client_address {};
    socklen_t client_addrLength = sizeof(struct sockaddr_in);
    int client_fd = accept(listen_fd, (struct sockaddr *)&client_address,
                           &client_addrLength);

    LOG_F(INFO, "client connection from: %s : % d(IP : port), clientfd = %d ",
          inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port),
          client_fd);

    add_fd(epoll_fd, client_fd, true);

    // 服务端用list保存用户连接
    clients_list.push_back(client_fd);
    LOG_F(INFO, "Add new clientfd = %d to epoll", client_fd);
    LOG_F(INFO, "Now there are %d clients int the IM_server room",
          (int)clients_list.size());

    // 服务端发送欢迎信息
    LOG_F(INFO, "welcome message");
    char message[BUF_SIZE];
    bzero(message, BUF_SIZE);
    sprintf(message, SERVER_WELCOME, client_fd);
    int ret = send(client_fd, message, BUF_SIZE, 0);
    if (ret < 0) {
      LOG_F(ERROR, "send error");
      exit(EXIT_FAILURE);
    }
  } else {
    //处理用户发来的消息，并广播，使其他用户收到信息
    int ret = broadcast_message_handler(socket_fd);
    if (ret < 0) {
      LOG_F(ERROR, "error");
      exit(EXIT_FAILURE);
    }
  }
}

#endif