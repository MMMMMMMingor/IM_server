//
// Created by Mingor on 2020/4/5.
//

#include <algorithm>
#include <common.hpp>
#include <server/session.h>

// ================= Session =================

int Session::get_socket_fd() { return m_socket_fd; }

Session::Time Session::get_last_keepalive() { return m_last_keepalive; }

const std::string &Session::get_username() { return m_username; }

std::string Session::get_room_name() { return m_room_name; }

std::mutex &Session::get_mutex() { return m_mutex; }

void Session::set_socket_fd(int socket_fd) { m_socket_fd = socket_fd; }

void Session::set_room_name(const std::string &name) { m_room_name = name; };

void Session::set_last_keepalive(Time last_keepalive) {
  m_last_keepalive = last_keepalive;
}

void Session::set_username(std::string username) {
  m_username = std::move(username);
}

// ================= SessionPool =================

// template <typename Lambda> void SessionPool::for_each(Lambda lambda) {
//  std::lock_guard<std::mutex> lock_guard(m_mutex);
//
//  std::for_each(m_session_map.begin(), m_session_map.end(),
//                [lambda](std::pair<uint64_t, Session *> kv) {
//                  lambda(kv.first, kv.second);
//                });
//}

bool SessionPool::contains(uint64_t session_id) {
  // 同步锁
  std::lock_guard<std::mutex> lock_guard(m_mutex);

  auto kv = m_session_map.find(session_id);
  return kv != m_session_map.end();
}

uint32_t SessionPool::get_current_count() {
  std::lock_guard<std::mutex> lock_guard(m_mutex);
  return m_session_map.size();
}

uint64_t SessionPool::add_session(Session *session) {
  // 同步锁
  std::lock_guard<std::mutex> lock_guard(m_mutex);

  uint64_t session_id = m_id_worker.get_next_id();
  m_session_map.insert(std::pair<uint64_t, Session *>(session_id, session));
  return session_id;
}

void SessionPool::remove_session(uint64_t session_id) {
  // 同步锁
  std::lock_guard<std::mutex> lock_guard(m_mutex);
  Session *session = m_session_map[session_id];
  close(session->get_socket_fd());
  m_session_map.erase(session_id);
  delete session;
}

void SessionPool::update_session(uint64_t session_id, Session *session) {
  std::lock_guard<std::mutex> lock_guard(m_mutex);

  delete m_session_map[session_id];
  m_session_map[session_id] = session;
}

Session *SessionPool::find_session(uint64_t session_id) {
  // 同步锁
  std::lock_guard<std::mutex> lock_guard(m_mutex);

  auto kv = m_session_map.find(session_id);
  if (kv != m_session_map.end()) {
    return kv->second;
  } else {
    return nullptr;
  }
}
