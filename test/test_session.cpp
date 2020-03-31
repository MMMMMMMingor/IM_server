//
// Created by Mingor on 2020/3/30.
//
#include "common.hpp"
#include <iostream>
#include <server/session.hpp>

int main(int argc, char *argv[]) {

  SessionPool session_pool;

  Session session{5, getTime(), "mingor"};
  uint64_t session_id = session_pool.add_session(session);

  Session *session2 = session_pool.find_session(session_id);
  std::cout << session.get_socket_fd() << session.get_username()
            << session.get_last_keepalive() << std::endl;

  Session session3{10, getTime(), "firefly"};
  session_pool.update_session(session_id, session3);

  Session *session4 = session_pool.find_session(session_id);
  std::cout << session4->get_socket_fd() << session4->get_username()
            << session4->get_last_keepalive() << std::endl;

  //  session_pool.remove_session(session_id);

  if (Session *session5 = session_pool.find_session(session_id))
    std::cout << session5->get_socket_fd() << session5->get_username()
              << session5->get_last_keepalive() << std::endl;

  session_pool.for_each([](uint64_t id, Session *session) {
    std::cout << id << session->get_socket_fd() << session->get_username()
              << session->get_last_keepalive() << std::endl;
  });

  std::cout << session_pool.contains(session_id) << std::endl;

  std::cout << session_pool.contains(0) << std::endl;

  return 0;
}