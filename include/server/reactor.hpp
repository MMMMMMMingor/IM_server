#ifndef REACTOR_H
#define REACTOR_H

#include <server/keepalive.h>
#include <server/threadpool.h>

class Reactor {

public:
  Reactor(int min, int max, int size, int check, int invalid);

  ~Reactor();

  /**
   * 初始化 Reactor
   */
  void init();

  /**
   * 事件分配器
   * @param event         事件
   * @param listen_fd     监听 socket 文件描述符
   * @param epoll_fd      epoll socket 文件描述符
   */
  bool dispatch(struct epoll_event event, int listen_fd, int epoll_fd);

private:
  ThreadPool *m_thread_pool;
  SessionPool m_session_pool;
  KeepAlive *m_keep_alive;
  std::unordered_map<std::string, HistoryMessage *> msgs;
  // 持久化每一位 用户的信息, 姓名加上房间号 才能获取到 ，
  std::unordered_map< std::string , user_msg *> users_info; //不用指针

};

#endif // REACTOR_HPP