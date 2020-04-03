//
// Created by Mingor on 2020/3/30.
//

#ifndef SESSION_HPP
#define SESSION_HPP

#include "message.pb.h"
#include "snowflake.hpp"
#include <chrono>
#include <exception>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>

/**
 * 会话 session
 */
class Session {
public:
    using Time = decltype(std::chrono::system_clock::now());

public:
    Session() = default;

    Session(const Session &rhs) = default;

    ~Session() = default;

    Session(int fd, Time last_keepalive, std::string username)
            : m_socket_fd(fd), m_last_keepalive(last_keepalive),
              m_username(std::move(username)) {}

    int get_socket_fd() { return m_socket_fd; }

    Time get_last_keepalive() { return m_last_keepalive; }

    const std::string &get_username() { return m_username; }

    void set_socket_fd(int socket_fd) { m_socket_fd = socket_fd; }

    void set_room_name(std::string name){this->room_name = name;};

    void set_last_keepalive(Time last_keepalive) {
        m_last_keepalive = last_keepalive;
    }

    void set_username(std::string username) { m_username = std::move(username); }

private:
    std::string room_name;
    int m_socket_fd{};
    Time m_last_keepalive;
    std::string m_username;
};

/**
 * 会话池
 */
class SessionPool {
public:
    //  using Lambda = void (*)(uint64_t, Session);

public:
    SessionPool() = default;

    ~SessionPool() = default;

    template<typename Lambda>
    void for_each(Lambda lambda) {
        std::lock_guard<std::mutex> lock_guard(m_mutex);

        std::for_each(m_session_map.begin(), m_session_map.end(),
                      [lambda](std::pair<uint64_t, Session *> kv) {
                          lambda(kv.first, kv.second);
                      });
    }

    bool contains(uint64_t session_id) {
        // 同步锁
        std::lock_guard<std::mutex> lock_guard(m_mutex);

        auto kv = m_session_map.find(session_id);
        return kv != m_session_map.end();
    }

    uint32_t get_current_count() { return m_session_map.size(); }

    uint64_t add_session(Session *session) {
        // 同步锁
        std::lock_guard<std::mutex> lock_guard(m_mutex);

        uint64_t session_id = m_id_worker.get_next_id();
        m_session_map.insert(std::pair<uint64_t, Session *>(session_id, session));
        return session_id;
    }

    void remove_session(uint64_t session_id) {
        // 同步锁
        std::lock_guard<std::mutex> lock_guard(m_mutex);
        Session *session = m_session_map[session_id];
        close(session->get_socket_fd());
        m_session_map.erase(session_id);
        delete session;
    }

    void update_session(uint64_t session_id, const Session &session) {
        m_session_map[session_id] = new Session{session};
    }

    Session *find_session(uint64_t session_id) {
        // 同步锁
        std::lock_guard<std::mutex> lock_guard(m_mutex);

        auto kv = m_session_map.find(session_id);
        if (kv != m_session_map.end()) {
            return kv->second;
        } else {
            return nullptr;
        }
    }

private:
    IdWorker m_id_worker{1, 1};
    std::mutex m_mutex{};
    std::unordered_map<uint64_t, Session *> m_session_map{};
};

#endif // SESSION_HPP
