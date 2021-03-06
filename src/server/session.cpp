//
// Created by Mingor on 2020/4/5.
//

#include <algorithm>
#include <common.h>
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

void Session::add_msg_id() {
    this->m_room_msg_id++; // 不会有重名的用户，加锁了
}

int Session::get_msg_id() {
    return this->m_room_msg_id;
}

void Session::set_msg_id(int id) {
    this->m_room_msg_id = id;
}

// ================= SessionPool =================

SessionPool *SessionPool::m_instance = nullptr;

SessionPool *SessionPool::get_instance() {
  if (m_instance == nullptr) {
    m_instance = new SessionPool;
  }
  return m_instance;
}

bool SessionPool::contains(uint64_t session_id) {
    // 同步锁
//    std::lock_guard<std::mutex> lock_guard(m_mutex);
    std::shared_lock<std::shared_mutex> lock(s_lock);
    auto kv = m_session_map.find(session_id);
    return kv != m_session_map.end();
}

uint32_t SessionPool::get_current_count() {
    std::shared_lock<std::shared_mutex> lock(s_lock);
    return m_session_map.size();
}

uint64_t SessionPool::add_session(Session *session) {
    // 同步锁
    //std::lock_guard<std::mutex> lock_guard(m_mutex);
    std::shared_lock<std::shared_mutex> lock(s_lock);

    uint64_t session_id = m_id_worker.get_next_id();
    m_session_map.insert(std::pair<uint64_t, Session *>(session_id, session));
    return session_id;
}

void SessionPool::remove_session(uint64_t session_id) {
    // 同步锁
    //std::lock_guard<std::mutex> lock_guard(m_mutex);
    //读写锁
    std::unique_lock<std::shared_mutex> lock(s_lock);
    Session *session = m_session_map[session_id];
    close(session->get_socket_fd());
    m_session_map.erase(session_id);
    delete session;
}

void SessionPool::update_session(uint64_t session_id, Session *session) {
    // std::lock_guard<std::mutex> lock_guard(m_mutex);
    std::unique_lock<std::shared_mutex> lock(s_lock);
    delete m_session_map[session_id];
    m_session_map[session_id] = session;
}

Session *SessionPool::find_session(uint64_t session_id) {
    // 同步锁
    //std::lock_guard<std::mutex> lock_guard(m_mutex);
    std::shared_lock<std::shared_mutex> lock(s_lock);
    auto kv = m_session_map.find(session_id);
    if (kv != m_session_map.end()) {
        return kv->second;
    } else {
        return nullptr;
    }
}

uint64_t SessionPool::find_session_id_by_fd(int fd) {
  // 同步锁
    //std::lock_guard<std::mutex> lock_guard(m_mutex);
    std::shared_lock<std::shared_mutex> lock(s_lock);
    for (auto &s : m_session_map)
        if (s.second->get_socket_fd() == fd) return s.first;
    return -1;
}
