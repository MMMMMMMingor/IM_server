#ifndef KEEPALIVE_HPP
#define KEEPALIVE_HPP

#include "common.hpp"
#include "loguru.hpp"
#include <chrono>
#include <thread>

// 必须使用 new 创建 ！！！
class KeepAlive {
public:
  using Munites = std::chrono::minutes;

  KeepAlive(int check_interval, int invalid_interval)
      : m_check_interval{check_interval}, m_invalid_interval{invalid_interval} {
  }

  ~KeepAlive() { m_finished = true; }

  /**
   * 开始心跳检测
   * @param ctx
   */
  void init(SessionPool *session_pool) {
    LOG_F(INFO, "开始心跳检测~~~");
    auto func = [this, session_pool]() {
      while (!m_finished) {
        std::this_thread::sleep_for(m_check_interval);
        keepalive_check(session_pool);
      }
    };
    std::thread t{func};

    t.detach();
  }

private:
  /**
   * 心跳检测：
   * 过期的session：清除
   * 未过期的session：发送心跳
   * @param ctx
   */
  void keepalive_check(SessionPool *session_pool) {
    im_message::Message message;
    message.set_type(im_message::HeadType::KEEPALIVE_REQUEST);

    auto keepalive = [this, &message, session_pool](uint64_t session_id,
                                                    Session *session) {
      Session::Time now = std::chrono::system_clock::now();

      Munites diff = std::chrono::duration_cast<Munites>(
          now - session->get_last_keepalive());

      // 如果超时，则移除session
      if (diff.count() > m_invalid_interval.count()) {
        LOG_F(INFO, "%s 心跳超时，服务器主动断开连接",
              session->get_username().c_str());
        session_pool->remove_session(session_id);
      } else {
        LOG_F(INFO, "发送心跳包 ---> %s ", session->get_username().c_str());
        message.SerializeToFileDescriptor(session->get_socket_fd());
      }
    };

    session_pool->for_each(keepalive);
  }

private:
  std::chrono::minutes m_check_interval;   // 心跳检测间隔
  std::chrono::minutes m_invalid_interval; // 判定失效间隔
  volatile bool m_finished{false};
};

#endif