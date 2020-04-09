#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include "base64.h"
#include "sha1.h"
#include "websocket_protocol.h"
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <sstream>

class Websocket_Handler {
public:
  explicit Websocket_Handler(int fd);
  ~Websocket_Handler() = default;
  int process();
  inline char *get_buff();

private:
  int handshake();
  int send_data(const char *buff);

private:
  int fd_;
  char buff_[2048];
  WebSocket_Protocol::WS_Status status_;
};

inline char *Websocket_Handler::get_buff() { return buff_; }

#endif
