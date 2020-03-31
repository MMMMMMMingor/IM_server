#ifndef CLIENT_LOGIN_HANDLER_HPP
#define CLIENT_LOGIN_HANDLER_HPP
/**
 * 用户登录
 * @param Context       上下文
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
  notification_all(ctx, login_notification);

  if (!success) {
    LOG_F(ERROR, "send error");
    exit(EXIT_FAILURE);
  }
}

#endif