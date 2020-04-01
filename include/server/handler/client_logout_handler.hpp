#ifndef CLIENT_LOGOUT_HANDLER_HPP
#define CLIENT_LOGOUT_HANDLER_HPP

/**
 * 用户下线
 * @param client_fd
 */
void client_logout_handler(Context ctx, const im_message::Message &message) {
  int client_fd = ctx.event.data.fd;

  im_message::Message response;
  response.set_type(im_message::HeadType::LOGOUT_RESPONSE);
  response.SerializeToFileDescriptor(client_fd);

  ctx.session_pool.remove_session(message.session_id());

  std::string notice{"用户xxx下线"};
  notification_all(ctx, notice);

  LOG_F(INFO, "ClientID = %d closed. now there are %d client in the char room",
        client_fd, ctx.session_pool.get_current_count());
}

#endif