//
// Created by Mingor on 2020/4/9.
//
#include "loguru.hpp"
#include "server/channel.h"
#include "server/handler.h"
#include "server/websocket_protocol.h"

inline int send_data(int fd, const char *buff) {
  return write(fd, buff, strlen(buff));
}

inline int handshake(Channel *channel) {
  LOG_F(INFO, "fd: %d, handshake", channel->fd_);

  std::string request = std::string(channel->buff_);
  std::string response;

  channel->status_ = WebSocket_Protocol::get_response_http(request, response);

  channel->clear_buff();

  return send_data(channel->fd_, response.c_str());
}

void websocket_handler(Context &ctx, im_message::Message &in_message) {
  Channel *channel = ctx.channel;

  // TODO 未解决半包、粘包问题！！！
  if (channel->buff_len_ = read(channel->fd_, channel->buff_, 2048);
      channel->buff_len_ <= 0) {
    LOG_F(ERROR, "read data error");
    close(channel->fd_);
  }

  // websocket 未握手
  if (channel->status_ ==
      WebSocket_Protocol::WS_Status::WEBSOCKET_UNCONNECTED) {
    handshake(channel);
    ctx.channel->terminate();
    return;
  }

  std::string out_message;
  channel->frame_type_ = WebSocket_Protocol::ws_encode_frame(
      "hello websocket", out_message,
      WebSocket_Protocol::WS_FrameType::WS_TEXT_FRAME);

  send_data(channel->fd_, out_message.c_str());

  ctx.channel->terminate();
}