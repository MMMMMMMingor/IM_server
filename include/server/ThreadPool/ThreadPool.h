//
// Created by Firefly on 2020/3/30.
//

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <sys/epoll.h>
#include <sys/types.h>

using Task = void (*)(epoll_event, int, int);

struct ARG {
  epoll_event event;
  int listen_fd;
  int epoll_fd;
  ARG(epoll_event e, int lf, int ef) : event(e), listen_fd(lf), epoll_fd(ef) {}
};

class ThreadPool {
private:
  //定义任务队列
  std::queue<std::pair<Task, ARG>> m_task_queue;
  //最大线程数量
  size_t m_max_count;
  //最小线程数量
  size_t m_min_count;
  //当前线程数量
  volatile size_t m_cur_count;
  //最大任务数量
  size_t m_max_queue_size;
  //条件变量
  std::condition_variable m_condition_variable;
  //互斥量
  std::mutex m_mutex;

  //线程入口
  static void *start(void *);

public:
  //构造函数,确定线程池的线程数
  ThreadPool(size_t min, size_t max, size_t size);

  //析构函数
  ~ThreadPool();

  //添加任务
  bool addTask(Task, ARG);
};

#endif // THREAD_POOL_H
