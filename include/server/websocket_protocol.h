#ifndef WEBSOCKET_PROTOCOL_H
#define WEBSOCKET_PROTOCOL_H

#include <string>

using std::string;

namespace WebSocket_Protocol {
enum WS_Status {
  WEBSOCKET_UNCONNECTED = 0,
  WEBSOCKET_HANDSHAKE = 1,
};

enum WS_FrameType {
  WS_EMPTY_FRAME = 0xF0,
  WS_ERROR_FRAME = 0xF1,
  WS_TEXT_FRAME = 0x01,
  WS_BINARY_FRAME = 0x02,
  WS_PING_FRAME = 0x09,
  WS_PONG_FRAME = 0x0A,
  WS_OPENING_FRAME = 0xF3,
  WS_CLOSING_FRAME = 0x08
};

WS_Status get_response_http(string &request, string &response);

WS_FrameType ws_decode_frame(string inFrame,
                             string &outMessage); //解码帧

WS_FrameType ws_encode_frame(string inMessage, string &outFrame,
                             enum WS_FrameType frameType); //编码帧打包
} // namespace WebSocket_Protocol

#endif