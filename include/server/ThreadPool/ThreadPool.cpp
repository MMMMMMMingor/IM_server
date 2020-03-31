//
// Created by Firefly on 2020/3/30.
//

#include "ThreadPool.h"
#include <iostream>
#include <loguru.hpp>
#include <pthread.h>
#include <thread>
#include <utility>

//构造函数,确定线程池的线程数
ThreadPool::ThreadPool(size_t min, size_t max, size_t size)
    : m_mutex{}, m_condition_variable{}, m_cur_count{0}, m_min_count{min},
      m_max_count{max}, m_max_queue_size{size} {

  LOG_F(INFO, "main pthread pid is %d", std::this_thread::get_id());
}

ThreadPool::~ThreadPool() { LOG_F(INFO, "ThreadPool is destroyed!!"); }

bool ThreadPool::addTask(Task task, ARG arg) {
  // 首先先获取锁
  std::lock_guard<std::mutex> lock_guard(m_mutex);

  if (m_cur_count < m_max_count) {
    m_cur_count++;
    std::thread t(start, this);
    t.detach();
  }

  if (m_task_queue.size() >= this->m_max_queue_size) {
    LOG_F(WARNING, "the task queue is full!!!,some task will be rejected");
    return false; // 返回错误
  }

  this->m_task_queue.push(std::make_pair(task, arg));

  m_condition_variable.notify_one();

  return true;
}

// 静态函数， 非成员函数调用
void *ThreadPool::start(void *arg) {
  ThreadPool *pool = (ThreadPool *)arg;
  LOG_F(INFO, "\"create thread %d\"", std::this_thread::get_id());
  while (true) {
    std::unique_lock<std::mutex> unique_lock(pool->m_mutex);

    while (pool->m_task_queue.empty()) {
      if (pool->m_cur_count > pool->m_min_count) {
        pool->m_cur_count--;
        LOG_F(INFO, "thread: %d\t No task and exit.",
              ((unsigned)pthread_self() % 100));
        return nullptr;
      }
      LOG_F(INFO, "thread: %d\tNo task go to sleep!!!",
            ((unsigned)pthread_self() % 100));
      pool->m_condition_variable.wait(unique_lock);
    }
    std::pair<Task, ARG> task = pool->m_task_queue.front();
    pool->m_task_queue.pop();
    unique_lock.unlock();

    task.first(task.second.event, task.second.listen_fd, task.second.epoll_fd);
  }
}
