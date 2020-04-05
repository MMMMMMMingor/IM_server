#include <server/handler.h>

/**
 * 用户下线
 * @param ctx
 * @param in_message
 */
void client_logout_handler(Context &ctx, im_message::Message &in_message) {
  if (im_message::HeadType::LOGOUT_REQUEST != in_message.type())
    return;

  int client_fd = ctx.event.data.fd;
  uint64_t session_id = in_message.session_id();
  std::string username =
      ctx.session_pool.find_session(session_id)->get_username();

  im_message::Message response;
  response.set_type(im_message::HeadType::LOGOUT_RESPONSE);
  response.set_session_id(session_id);
  send_message(ctx, response);

  ctx.session_pool.remove_session(session_id);
  close(client_fd);

  std::string notice{"用户 "};
  notice += username;
  notice += " 下线";
  notification_all(ctx, notice);

  LOG_F(INFO, "ClientID = %d closed. now there are %d client in the char room",
        client_fd, ctx.session_pool.get_current_count());
}
