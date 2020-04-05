#include <server/handler.h>

/**
 * 用户登录
 * @param ctx
 * @param in_message
 */
void client_login_handler(Context &ctx, im_message::Message &in_message) {
  if (im_message::HeadType::LOGIN_REQUEST != in_message.type()) {
    return;
  }

  int client_fd = ctx.event.data.fd;
  const auto &login_request = in_message.loginrequest();
  // 获取房间号和 用户名
  const std::string &username(login_request.username());
  const std::string &pwd = login_request.password();

  // 添加索引
  if (ctx.msg_map.find(pwd) == ctx.msg_map.end()) {
    ctx.msg_map[pwd] = new HistoryMessage();
  }

  // TODO 没有考虑， 同名的情况， 之后加一个同名的判断

  // 添加session
  auto *session = new Session{};
  session->set_username(login_request.username());
  session->set_last_keepalive(std::chrono::system_clock::now());
  session->set_socket_fd(client_fd);
  //增添 room_name
  session->set_room_name(pwd);

  uint64_t session_id = ctx.session_pool.add_session(session);

  LOG_F(INFO, "用户登录：%s : %s", username.c_str(),
        login_request.password().c_str());

  LOG_F(INFO, "Now there are %d clients int the IM_server room",
        ctx.session_pool.get_current_count());

  // 服务端发送欢迎信息
  std::string welcome = "欢迎来到文明——聊天室! 您的 session ID : ";
  welcome += std::to_string(session_id);

  im_message::Message out_message;
  out_message.set_type(im_message::HeadType::LOGIN_RESPONSE);
  out_message.set_session_id(session_id);
  auto *login_response = new im_message::LoginResponse;
  login_response->set_result(true);
  login_response->set_info(welcome);
  out_message.set_allocated_loginresponse(login_response);

  send_message(ctx, out_message);

  std::string login_notification = login_request.username();
  login_notification += " 登录";

  // pwd 作为 room id
  //  notification_by_room_id(ctx, login_notification, pwd);

  // 添加消息, Message 类型只存储了 名字和房间号， 提示信息还要封装
  ctx.msg_map[pwd]->add_one(out_message);
}
