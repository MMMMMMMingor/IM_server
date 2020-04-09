#include "server/websocket_handler.h"
#include "loguru.hpp"
#include "server/websocket_protocol.h"
#include <cstring>
#include <string>
#include <unistd.h>

Websocket_Handler::Websocket_Handler(int fd)
    : fd_(fd), buff_(),
      status_(WebSocket_Protocol::WS_Status::WEBSOCKET_UNCONNECTED) {}

int Websocket_Handler::process() {
  if (status_ == WebSocket_Protocol::WS_Status::WEBSOCKET_UNCONNECTED) {
    return handshake();
  }

  std::string out_string;
  WebSocket_Protocol::ws_decode_frame(std::string(buff_), out_string);
  LOG_F(INFO, "payload %s", out_string.c_str());

  memset(buff_, 0, sizeof(buff_));

  std::string out_message;
  WebSocket_Protocol::ws_encode_frame(
      "hello websocket", out_message,
      WebSocket_Protocol::WS_FrameType::WS_TEXT_FRAME);

  send_data(out_message.c_str());

  return 0;
}

int Websocket_Handler::handshake() {
  std::string request = std::string(buff_);
  std::string response;
  memset(buff_, 0, sizeof(buff_));

  status_ = WebSocket_Protocol::get_response_http(request, response);

  return send_data(response.c_str());
}

int Websocket_Handler::send_data(const char *buff) {
  return write(fd_, buff, strlen(buff));
}
