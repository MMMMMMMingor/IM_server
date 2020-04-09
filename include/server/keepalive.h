#ifndef KEEPALIVE_H
#define KEEPALIVE_H

#include <chrono>
#include <loguru.hpp>
#include <message.pb.h>
#include <server/session.h>
#include <thread>

// 必须使用 new 创建 ！！！
class KeepAlive {
public:
  using Munites = std::chrono::minutes;

  KeepAlive(int check_interval, int invalid_interval)
      : m_check_interval{check_interval}, m_invalid_interval{invalid_interval} {
  }

  ~KeepAlive() { m_finished = true; }

  void init();

private:
  void keepalive_check();

private:
  std::chrono::minutes m_check_interval;   // 心跳检测间隔
  std::chrono::minutes m_invalid_interval; // 判定失效间隔
  volatile bool m_finished{false};
};

#endif