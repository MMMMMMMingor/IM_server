//
// Created by Mingor on 2020/3/30.
//
#include "server/session.h"
#include <chrono>
#include <iostream>
#include <ratio>

using namespace std;

inline int distance(Session::Time time) {
  return chrono::duration_cast<chrono::seconds>(
             (chrono::system_clock::now() - time))
      .count();
}

inline void print(Session *session) {
  cout << session->get_socket_fd() << "\n"
       << session->get_username() << "\n"
       << distance(session->get_last_keepalive()) << "\n------------------\n";
}

int main(int argc, char *argv[]) {

  SessionPool *session_pool = SessionPool::get_instance();

  Session *session = new Session{5, chrono::system_clock::now(), "mingor"};
  uint64_t session_id = session_pool->add_session(session);
  session_pool->add_session(session);
  session_pool->add_session(session);
  session_pool->add_session(session);
  //
  //  Session *session2 = session_pool.find_session(session_id);
  //  print(session2);
  //
  //  Session session3{10, chrono::system_clock::now(), "firefly"};
  //  session_pool.update_session(session_id, session3);
  //
  //  Session *session4 = session_pool.find_session(session_id);
  //  print(session4);
  //

  //  session_pool.remove_session(session_id);

  //  if (Session *session5 = session_pool.find_session(session_id))
  //    print(session5);

  cout << session_pool->get_current_count() << endl;

  session_pool->remove_session(session_id);

  session_pool->for_each(
      [](uint64_t id, Session *session) {
        std::cout << id << "\n";
      });

  cout << session_pool->get_current_count() << endl;

  //  cout << session_pool.contains(session_id) << endl;
  //
  //  cout << session_pool.contains(0) << endl;

  return 0;
}