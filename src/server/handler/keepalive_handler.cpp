#include <server/handler.h>

/**
 * 接收心跳包
 * @param ctx
 * @param in_message
 */
void keepalive_handler(Context &ctx, im_message::Message &in_message) {
  if (im_message::HeadType::KEEPALIVE_RESPONSE != in_message.type())
    return;

  uint64_t session_id = in_message.session_id();
  auto session = SessionPool::get_instance()->find_session(session_id);

  std::string username = session->get_username();
  LOG_F(INFO, "收到来自  %s  的心跳包", username.c_str());

  session->set_last_keepalive(std::chrono::system_clock::now());
  send_message(ctx, in_message);
}
