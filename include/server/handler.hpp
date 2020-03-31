#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "loguru.hpp"
#include "message.pb.h"
#include "session.hpp"
#include <string>

/**
 * 广播消息
 * @param message
 */
void board_cast_message(Context &ctx, const im_message::Message &message) {

  auto lambda = [&message](uint64_t session_id, Session session) {
    bool success = message.SerializeToFileDescriptor(session.get_socket_fd());

    if (!success) {
      LOG_F(ERROR, "board cast error");
      //      exit(EXIT_FAILURE);
    }
  };

  ctx.session_pool.for_each(lambda);
}

/**
 * 消息通知
 * @param message 消息
 */
void notification(Context &ctx, const std::string &message) {
  im_message::Message response;
  response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
  auto *message_notification = new im_message::MessageNotification;
  message_notification->set_json(message);
  message_notification->set_timestamp(getTime());
  response.set_allocated_notification(message_notification);

  board_cast_message(ctx, response);
}

/**
 * 消息返回
 * @param message 消息
 */
void response(Context &ctx, const std::string &message) {
  im_message::Message response;
  response.set_type(im_message::HeadType::MESSAGE_RESPONSE);
  auto *message_response = new im_message::MessageResponse;
  message_response->set_info(message);
  response.set_allocated_messageresponse(message_response);

  response.SerializeToFileDescriptor(ctx.event.data.fd);
}

/**
 * 创建session会话
 * @param listen_fd
 * @param epoll_fd
 */
void create_session_handler(Context ctx) {
  struct sockaddr_in client_address {};
  socklen_t client_addrLength = sizeof(struct sockaddr_in);
  int client_fd = accept(ctx.listen_fd, (struct sockaddr *)&client_address,
                         &client_addrLength);

  LOG_F(INFO, "client connection from: %s : % d(IP : port), client_fd = %d ",
        inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port),
        client_fd);

  add_fd(ctx.epoll_fd, client_fd, true);
  LOG_F(INFO, "Add new client_fd = %d to epoll", client_fd);
}

/**
 * 用户登录
 * @param client_fd 用户socket描述符
 * @param login_request 消息
 */
void client_login_handler(Context ctx, const im_message::Message &message) {
  int client_fd = ctx.event.data.fd;
  const auto &login_request = message.loginrequest();

  LOG_F(INFO, "Now there are %d clients int the IM_server room",
        ctx.session_pool.get_current_count());

  LOG_F(INFO, "登录：%s : %s", login_request.username().c_str(),
        login_request.password().c_str());

  // 添加session
  Session session;
  session.set_username(login_request.username());
  session.set_last_keepalive(getTime());
  session.set_socket_fd(client_fd);
  uint64_t session_id = ctx.session_pool.add_session(session);

  // 服务端发送欢迎信息
  LOG_F(INFO, "welcome message");
  char welcome[BUF_SIZE];
  bzero(welcome, BUF_SIZE);
  sprintf(welcome, SERVER_WELCOME, client_fd);

  im_message::Message response;
  response.set_type(im_message::HeadType::LOGIN_RESPONSE);
  response.set_session_id(session_id);
  auto *login_response = new im_message::LoginResponse;
  login_response->set_result(true);
  login_response->set_info(welcome);
  response.set_allocated_loginresponse(login_response);

  bool success = response.SerializeToFileDescriptor(client_fd);

  std::string login_notification = login_request.username();
  login_notification += " 登录";
  notification(ctx, login_notification);

  if (!success) {
    LOG_F(ERROR, "send error");
    exit(EXIT_FAILURE);
  }
}

/**
 * 用户下线
 * @param client_fd
 */
void client_logout_handler(Context ctx, const im_message::Message &message) {
  int client_fd = ctx.event.data.fd;

  im_message::Message response;
  response.set_type(im_message::HeadType::LOGOUT_RESPONSE);
  response.SerializeToFileDescriptor(client_fd);

  close(client_fd);
  ctx.session_pool.remove_session(message.session_id());

  std::string notice{"用户xxx下线"};
  notification(ctx, notice);

  LOG_F(INFO, "ClientID = %d closed. now there are %d client in the char room",
        client_fd, ctx.session_pool.get_current_count());
}

/**
 * 转发消息handler
 * @param client_fd     客户端socket文件描述符
 * @param message       消息
 */
void transmit_message_handler(Context ctx, const im_message::Message &message) {

  int client_fd = ctx.event.data.fd;

  LOG_F(INFO, "read from client(clientID = %d)", client_fd);

  // 当前只有一个用户在线
  if (1 == ctx.session_pool.get_current_count()) {
    std::string notice(CAUTION);
    //通知
    response(ctx, notice);

    LOG_F(INFO, CAUTION);
  } else {
    // 消息转发
    uint64_t session_id = message.session_id();
    const std::string &username =
        ctx.session_pool.find_session(session_id).get_username();
    im_message::Message request;
    request.set_type(im_message::HeadType::MESSAGE_REQUEST);
    request.set_session_id(session_id);
    auto *message_request = new im_message::MessageRequest{};
    message_request->set_content(message.messagerequest().content());
    message_request->set_from_nick(username);
    request.set_allocated_messagerequest(message_request);

    board_cast_message(ctx, request);
  }
}

#endif // !HANDLER_HPP
