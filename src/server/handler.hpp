#ifndef HANDLE_HPP
#define HANDLE_HPP

#include "common.hpp"
#include "loguru.hpp"

/**
 * 处理用户登录事件
 * @param listen_fd
 * @param epoll_fd
 */
void client_login_handler(const int listen_fd,const int epoll_fd){
  struct sockaddr_in client_address {};
  socklen_t client_addrLength = sizeof(struct sockaddr_in);
  int client_fd = accept(listen_fd, (struct sockaddr *)&client_address,
                         &client_addrLength);

  LOG_F(INFO, "client connection from: %s : % d(IP : port), client_fd = %d ",
        inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port),
        client_fd);

  add_fd(epoll_fd, client_fd, true);

  // 服务端用list保存用户连接
  clients_list.push_back(client_fd);
  LOG_F(INFO, "Add new client_fd = %d to epoll", client_fd);
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
}

/**
 * 广播消息 handler
 * @param client_fd 客户端socket文件描述符
 */
void broadcast_message_handler(int client_fd) {
  char buf[BUF_SIZE];
  char message[BUF_SIZE];
  bzero(buf, BUF_SIZE);
  bzero(buf, BUF_SIZE);

  LOG_F(INFO, "read from client(clientID = %d)", client_fd);
  int len = recv(client_fd, buf, BUF_SIZE, 0);

  if (0 == len) {
    close(client_fd);
    clients_list.remove(client_fd);
    LOG_F(INFO,
          "ClientID = %d closed. now there are %d client in the char room",
          client_fd, (int)clients_list.size());
  } else {
    if (1 == clients_list.size()) {
      send(client_fd, CAUTION, strlen(CAUTION), 0);
    }

    sprintf(message, SERVER_REDIRECT_MESSAGE, client_fd, buf);

    for (const auto &socket : clients_list) {
      if (socket != client_fd) {
        if (send(socket, message, BUF_SIZE, 0) < 0) {
          LOG_F(ERROR, "board cast error");
          exit(EXIT_FAILURE);
        }
      }
    }
  }
}

#endif // !HANDLE_HPP
