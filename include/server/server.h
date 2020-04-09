//
// Created by Mingor on 2020/4/9.
//

#ifndef SERVER_H
#define SERVER_H

#include <common.h>
#include <loguru.hpp>
#include <server/common_util.hpp>
#include <server/reactor.hpp>
#include <yaml.hpp>

class Server {

public:
  Server();

  ~Server();

  void init();

  void init_reactor();

  void init_epoll();

  void configuration();

  void start();

private:
  int listener{};
  int epfd{};
  struct sockaddr_in serverAddr {};
  struct epoll_event *events{};
  std::string SERVER_IP;
  uint16_t SERVER_PORT{};
  int EPOLL_SIZE{};

  int THREAD_MIN_NUM{};
  int THREAD_MAX_NUM{};
  int THREAD_QUEUE_SIZE{};
  int KEEPALIVE_CHECK_INTERVAL{};
  int KEEPALIVE_INVALID_INTERVAL{};

  Reactor *reactor{};
};

#endif // #define SERVER_H
