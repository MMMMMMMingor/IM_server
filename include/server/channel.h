#ifndef CHANNEL_H
#define CHANNEL_H

#include <message.pb.h>
#include <server/context.hpp>

void check_login_handler(Context &ctx, im_message::Message &in_message);
void client_login_handler(Context &ctx, im_message::Message &in_message);
void client_logout_handler(Context &ctx, im_message::Message &in_message);
void create_connection_handler(Context &ctx, im_message::Message &in_message);
void in_bound_handler(Context &ctx, im_message::Message &in_message);
void keepalive_handler(Context &ctx, im_message::Message &in_message);
void transmit_message_handler(Context &ctx, im_message::Message &in_message);

class Channel {
public:
  using Handler = void (*)(Context &, im_message::Message &);

  Channel() = default;
  ~Channel() = default;

  void terminate();

  void pipeline(Context &ctx);

private:
  bool m_terminated{false};
  im_message::Message in_message{};
  std::vector<Handler> m_handlers{
      // 注意顺序不能随意更改！！！！
      create_connection_handler, in_bound_handler,      client_login_handler,
      check_login_handler,       client_logout_handler, keepalive_handler,
      transmit_message_handler};
};

#endif