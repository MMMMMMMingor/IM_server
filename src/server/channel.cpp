//
// Created by Mingor on 2020/4/5.
//
#include <server/channel.h>

void Channel::terminate() { m_terminated = true; }

void Channel::pipeline(Context &ctx) {
  for (Handler handler : m_handlers) {
    if (!m_terminated)
      handler(ctx, in_message);
  }
}
