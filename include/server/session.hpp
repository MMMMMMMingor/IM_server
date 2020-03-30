//
// Created by Mingor on 2020/3/30.
//

#ifndef SESSION_HPP
#define SESSION_HPP

#include "common.hpp"
#include "message.pb.h"
#include "snowflake.hpp"
#include <map>
#include <mutex>
#include <utility>

/**
 * 会话 session
 */
class Session {
public:
  Session() = default;

  Session(int fd, std::string last_keepalive, std::string username)
      : m_socket_fd(fd), m_last_keepalive(std::move(last_keepalive)),
        m_username(std::move(username)) {}

  int get_socket_fd() { return m_socket_fd; }

  std::string get_last_keepalive() { return m_last_keepalive; }

  std::string get_username() { return m_username; }

private:
  int m_socket_fd{};
  std::string m_last_keepalive;
  std::string m_username;
};

/**
 * 会话池
 */
class SessionPool {
public:
  SessionPool() = default;
  ~SessionPool() = default;

  uint64_t add_session(int fd, std::string &username) {
    // 同步锁
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    uint64_t session_id = m_id_worker.get_next_id();
    Session s{fd, getTime(), username};
    m_session_map.insert(std::pair<uint64_t, Session>(session_id, s));
    return session_id;
  }

  void remove_session(uint64_t session_id) { m_session_map.erase(session_id); }

  void update_session(uint64_t session_id, Session session) {
    remove_session(session_id);
    add_session(session);
  }

  Session find_session(uint64_t session_id) {
    auto s = m_session_map.find(session_id);
    if (s != m_session_map.end()) {
      return s->second;
    } else
      return {};
  }

private:
  IdWorker m_id_worker{1, 1};
  std::mutex m_mutex{};
  std::map<uint64_t, Session> m_session_map{};
};

#endif // SESSION_HPP
