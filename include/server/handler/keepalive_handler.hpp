#ifndef KEEPALIVE_HANDLER_HPP
#define KEEPALIVE_HANDLER_HPP

#include <chrono>

/**
 * 接收心跳包
 * @param ctx       上下文
 * @param message   消息
 */
void keepalive_handler(Context &ctx, const im_message::Message &message) {
  uint64_t session_id = message.session_id();
  std::string username =
      ctx.session_pool.find_session(session_id)->get_username();
  LOG_F(INFO, "收到来自  %s  的心跳包", username.c_str());

  Session *session = ctx.session_pool.find_session(session_id);
  session->set_last_keepalive(std::chrono::system_clock::now());
}

#endif