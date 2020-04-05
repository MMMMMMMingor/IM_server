//
// Created by Firefly on 2020/3/30.
//

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "common.hpp"
#include <condition_variable>
#include <cstddef>
#include <loguru.hpp>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <sys/epoll.h>
#include <sys/types.h>
#include <thread>
#include <utility>

using Task = void (*)(Context);

class ThreadPool {
public:
  //构造函数,确定线程池的线程数
  ThreadPool(size_t min, size_t max, size_t size);

  ~ThreadPool();

  //  template <typename Task>
  bool add_task(Task task, Context cxt);

private:
  // 静态函数， 非成员函数调用
  static void start(ThreadPool *pool);

private:
  // 是否结束
  bool *m_finished = new bool{false};
  //定义任务队列
  std::queue<std::pair<Task, Context>> m_task_queue;
  //最小线程数量
  const size_t m_min_count;
  //最大线程数量
  const size_t m_max_count;
  //当前线程数量
  volatile size_t m_cur_count{0};
  //最大任务数量
  size_t m_max_queue_size;
  //条件变量
  std::condition_variable m_condition_variable{};
  //互斥量
  std::mutex m_mutex{};
};

#endif // THREAD_POOL_H
