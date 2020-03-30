#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "common.hpp"
#include "loguru.hpp"
#include "message.pb.h"
#include <string>

/**
 * 广播消息
 * @param message
 */
void board_cast_message(const im_message::Message &message) {

  for (const auto &socket : clients_list) {
    LOG_F(INFO, "board cast %i", socket);

    bool success = message.SerializeToFileDescriptor(socket);

    if (!success) {
      LOG_F(ERROR, "board cast error");
      exit(EXIT_FAILURE);
    }
  }
}

/**
 * 消息通知
 * @param message 消息
 */
void notification(const std::string &message) {
  im_message::Message response;
  response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
  auto *message_notification = new im_message::MessageNotification;
  message_notification->set_json(message);
  message_notification->set_timestamp(getTime());
  response.set_allocated_notification(message_notification);

  board_cast_message(response);
}

/**
 * 消息返回
 * @param message 消息
 */
void message_response(const std::string &message) {
  im_message::Message response;
  response.set_type(im_message::HeadType::MESSAGE_RESPONSE);
  auto *message_response = new im_message::MessageResponse;
  message_response->set_info(message);
  response.set_allocated_messageresponse(message_response);

  board_cast_message(response);
}

/**
 * 创建session会话
 * @param listen_fd
 * @param epoll_fd
 */
void create_session_handler(const int listen_fd, const int epoll_fd) {
  struct sockaddr_in client_address {};
  socklen_t client_addrLength = sizeof(struct sockaddr_in);
  int client_fd =
      accept(listen_fd, (struct sockaddr *)&client_address, &client_addrLength);

  LOG_F(INFO, "client connection from: %s : % d(IP : port), client_fd = %d ",
        inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port),
        client_fd);

  add_fd(epoll_fd, client_fd, true);
  LOG_F(INFO, "Add new client_fd = %d to epoll", client_fd);
}

/**
 * 用户登录
 * @param client_fd 用户socket描述符
 * @param login_request 消息
 */
void client_login_handler(int client_fd,
                          const im_message::LoginRequest &login_request) {

  LOG_F(INFO, "Now there are %d clients int the IM_server room",
        (int)clients_list.size());

  LOG_F(INFO, "登录：%s : %s", login_request.username().c_str(),
        login_request.password().c_str());

  // 服务端用list保存用户连接
  clients_list.push_back(client_fd);

  // 服务端发送欢迎信息
  LOG_F(INFO, "welcome message");
  char message[BUF_SIZE];
  bzero(message, BUF_SIZE);
  sprintf(message, SERVER_WELCOME, client_fd);

  im_message::Message response;
  response.set_type(im_message::HeadType::LOGIN_RESPONSE);
  auto *login_response = new im_message::LoginResponse;
  login_response->set_result(true);
  login_response->set_info(message);
  response.set_allocated_loginresponse(login_response);

  bool success = response.SerializeToFileDescriptor(client_fd);

  std::string login_notification = login_request.username();
  login_notification += " 登录";
  notification(login_notification);

  if (!success) {
    LOG_F(ERROR, "send error");
    exit(EXIT_FAILURE);
  }
}

/**
 * 用户下线
 * @param client_fd
 */
void client_logout_handler(int client_fd) {
  im_message::Message response;
  response.set_type(im_message::HeadType::LOGOUT_RESPONSE);
  response.SerializeToFileDescriptor(client_fd);

  close(client_fd);
  clients_list.remove(client_fd);

  std::string message{"用户xxx下线"};
  notification(message);

  LOG_F(INFO, "ClientID = %d closed. now there are %d client in the char room",
        client_fd, (int)clients_list.size());
}

/**
 * 转发消息 handler
 * @param client_fd 客户端socket文件描述符
 * @param message 消息
 */
void transmit_message_handler(
    int client_fd, const im_message::MessageRequest &message_request) {

  LOG_F(INFO, "read from client(clientID = %d)", client_fd);

  // 当前只有一个用户在线
  if (1 == clients_list.size()) {
    std::string message(CAUTION);
    //通知
    notification(message);

    LOG_F(INFO, CAUTION);
  } else {
    // 消息转发
    std::string redirect = message_request.from();
    redirect += " -> ";
    redirect += message_request.content();
    notification(redirect);
  }
}

#endif // !HANDLER_HPP
