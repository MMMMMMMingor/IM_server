#include <server/handler.h>

/**
 * 转发消息handler
 * @param ctx
 * @param in_message
 */
void transmit_message_handler(Context &ctx, im_message::Message &in_message) {
  if (im_message::HeadType::MESSAGE_REQUEST != in_message.type())
    return;

  int client_fd = ctx.event.data.fd;
  uint64_t session_id = in_message.session_id();
  Session *session = ctx.session_pool.find_session(session_id);

  LOG_F(INFO, "read from client(clientID = %d)", client_fd);

  // 当前只有一个用户在线
  if (1 == ctx.session_pool.get_current_count()) {
    std::string notice("聊天室仅有您一个人。。。");
    //通知
    notification_one(ctx, session_id, notice);

    LOG_F(INFO, "聊天室当前只有一位用户");
  } else {
    // 消息转发

    std::string username = session->get_username();

    im_message::Message out_message;
    out_message.set_type(im_message::HeadType::MESSAGE_REQUEST);
    out_message.set_session_id(session_id);
    auto *message_request = new im_message::MessageRequest{};
    message_request->set_content(in_message.messagerequest().content());
    message_request->set_from_nick(username);
    out_message.set_allocated_messagerequest(message_request);

    board_cast_message(ctx, out_message);
  }
}
