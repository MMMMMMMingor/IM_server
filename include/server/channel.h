#ifndef CHANNEL_H
#define CHANNEL_H

#include "message.pb.h"
#include "server/context.hpp"
#include "server/websocket_protocol.h"

class Channel {
public:
  int fd_;
  int buff_len_{0};
  char buff_[2048];
  im_message::Message in_message{};
  WebSocket_Protocol::WS_Status status_{
      WebSocket_Protocol::WS_Status::WEBSOCKET_UNCONNECTED};
  WebSocket_Protocol::WS_FrameType frame_type_{
      WebSocket_Protocol::WS_FrameType::WS_EMPTY_FRAME};

public:
  using Handler = void (*)(Context &, im_message::Message &);

  explicit Channel(int fd);
  ~Channel() = default;

  void terminate();

  void pipeline(Context &ctx);

  void clear_buff();

private:
  bool m_terminated{false};
  static std::vector<Handler> m_handlers;
};

#endif