//
// Created by Mingor on 2020/3/30.
//

#ifndef SESSION_H
#define SESSION_H

#include "snowflake.h"
#include <algorithm>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

/**
 * 会话 session
 */
class Session {
public:
  using Time = decltype(std::chrono::system_clock::now());

public:
  Session() = default;

  Session(Session &&rhs) = default;

  ~Session() = default;

  Session(int fd, Time last_keepalive, std::string username)
      : m_socket_fd(fd), m_last_keepalive(last_keepalive),
        m_username(std::move(username)) {}

  int get_socket_fd();

  Time get_last_keepalive();

  const std::string &get_username();

  std::string get_room_name();

  std::mutex &get_mutex();

  void set_socket_fd(int socket_fd);

  void set_room_name(const std::string &name);

  void set_last_keepalive(Time last_keepalive);

  void set_username(std::string username);

  void add_msg_id();

  void set_msg_id(int);

  int get_msg_id();

private:
  std::string m_room_name;
  int m_socket_fd{};
  Time m_last_keepalive;
  std::string m_username;
  int m_room_msg_id{0}; // 每发送一条消息， 这里的房间号消息就加一
  std::mutex m_mutex{};
};

class shared_lock;

class shared_lock;

/**
 * 会话池
 */
class SessionPool {
public:
  static SessionPool *get_instance();

public:
  template <typename Lambda> void for_each(Lambda lambda) {
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    std::for_each(m_session_map.begin(), m_session_map.end(),
                  [lambda](std::pair<uint64_t, Session *> kv) {
                    lambda(kv.first, kv.second);
                  });
  }

  bool contains(uint64_t session_id);

  uint32_t get_current_count();

  uint64_t add_session(Session *session);

  void remove_session(uint64_t session_id);

  void update_session(uint64_t session_id, Session *session);

  Session *find_session(uint64_t session_id);

  uint64_t find_session_id_by_fd(int fd);

private:
  SessionPool() = default;

  ~SessionPool() = default;

  static SessionPool *m_instance;
  IdWorker m_id_worker{1, 1};
  std::mutex m_mutex{};
  mutable std::shared_mutex s_lock{};
  std::unordered_map<uint64_t, Session *> m_session_map{};
};

#endif // SESSION_H
