#ifndef TRANSMIT_MESSAGE_HANDLER_HPP
#define TRANSMIT_MESSAGE_HANDLER_HPP

/**
 * 转发消息handler
 * @param Context       上下文
 * @param message       消息
 */
void transmit_message_handler(Context ctx, const im_message::Message &message) {

  int client_fd = ctx.event.data.fd;

  LOG_F(INFO, "read from client(clientID = %d)", client_fd);

  // 当前只有一个用户在线
  if (1 == ctx.session_pool.get_current_count()) {
    std::string notice(CAUTION);
    //通知
    notification_one(ctx, notice);

    LOG_F(INFO, CAUTION);
  } else {
    // 消息转发
    uint64_t session_id = message.session_id();
    Session *session = ctx.session_pool.find_session(session_id);
    std::string username = session->get_username();

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

#endif