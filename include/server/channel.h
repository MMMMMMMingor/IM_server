#ifndef CHANNEL_H
#define CHANNEL_H

#include "server/context.hpp"
#include <message.pb.h>

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
  static std::vector<Handler> m_handlers;
};

#endif