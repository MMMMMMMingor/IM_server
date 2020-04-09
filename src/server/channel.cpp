//
// Created by Mingor on 2020/4/5.
//
#include "server/channel.h"
#include "server/handler.h"

std::vector<Channel::Handler> Channel::m_handlers = {
    // 注意顺序不能随意更改！！！！
    create_connection_handler, in_bound_handler,      client_login_handler,
    check_login_handler,       client_logout_handler, keepalive_handler,
    transmit_message_handler};

void Channel::terminate() { m_terminated = true; }

void Channel::pipeline(Context &ctx) {
  for (Handler handler : m_handlers) {
    if (!m_terminated)
      handler(ctx, in_message);
  }
}
