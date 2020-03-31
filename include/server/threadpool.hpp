//
// Created by Firefly on 2020/3/30.
//

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

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
  ThreadPool(size_t min, size_t max, size_t size)
      : m_mutex{}, m_condition_variable{}, m_cur_count{0}, m_min_count{min},
        m_max_count{max}, m_max_queue_size{size} {

    LOG_F(INFO, "main pthread pid is %d", (unsigned)pthread_self() % 100);
  }

  ~ThreadPool() { LOG_F(INFO, "ThreadPool is destroyed!!"); }

  bool addTask(Task task, Context cxt) {
    // 首先先获取锁
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    if (m_cur_count < m_max_count) {
      m_cur_count++;
      std::thread t(ThreadPool::start, this);
      t.detach();
    }

    if (m_task_queue.size() >= this->m_max_queue_size) {
      LOG_F(WARNING, "the task queue is full!!!,some task will be rejected");
      return false; // 返回错误
    }

    this->m_task_queue.push(std::make_pair(task, cxt));

    m_condition_variable.notify_one();

    return true;
  }

private:
  // 静态函数， 非成员函数调用
  static void *start(ThreadPool *pool) {
    LOG_F(INFO, "\"create thread %d\"", (unsigned)pthread_self() % 100);
    while (true) {
      std::unique_lock<std::mutex> unique_lock(pool->m_mutex);

      while (pool->m_task_queue.empty()) {
        if (pool->m_cur_count > pool->m_min_count) {
          pool->m_cur_count--;
          LOG_F(INFO, "thread: %d\t No task and exit.",
                ((unsigned)pthread_self() % 100));
          return nullptr;
        }
        LOG_F(INFO, "thread: %d\tNo task and go to sleep!!!",
              ((unsigned)pthread_self() % 100));
        pool->m_condition_variable.wait(unique_lock);
      }
      std::pair<Task, Context> task = pool->m_task_queue.front();
      pool->m_task_queue.pop();
      unique_lock.unlock();

      task.first(task.second);
    }
  }

private:
  //定义任务队列
  std::queue<std::pair<Task, Context>> m_task_queue;
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
};

#endif // THREAD_POOL_HPP
