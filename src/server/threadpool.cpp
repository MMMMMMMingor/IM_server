//
// Created by Mingor on 2020/4/5.
//
#include <server/threadpool.h>

using Task = void (*)(Context);

//构造函数,确定线程池的线程数
ThreadPool::ThreadPool(size_t min, size_t max, size_t size)
    : m_min_count{min}, m_max_count{max}, m_max_queue_size{size} {

  LOG_F(INFO, "main pthread pid is %d", (unsigned)pthread_self() % 100);
}

ThreadPool::~ThreadPool() {
  *m_finished = true;
  LOG_F(INFO, "ThreadPool is destroyed!!");
}

//  template <typename Task>
bool ThreadPool::add_task(Task task, Context cxt) {
  // 首先先获取锁
  std::lock_guard<std::mutex> lock_guard(m_mutex);

  this->m_task_queue.push(std::make_pair(task, cxt));

  if (m_cur_count < m_task_queue.size() && m_cur_count < m_max_count) {
    m_cur_count++;
    std::thread t(ThreadPool::start, this);
    t.detach();
  }

  if (m_task_queue.size() >= this->m_max_queue_size) {
    LOG_F(WARNING, "the task queue is full!!!,some task will be rejected");
    return false; // 返回错误
  }

  m_condition_variable.notify_one();

  return true;
}

// 静态函数， 非成员函数调用
void ThreadPool::start(ThreadPool *pool) {
  LOG_F(INFO, "\"create thread %d\"", ((unsigned)pthread_self() % 100));
  while (!*(pool->m_finished)) {
    std::unique_lock<std::mutex> unique_lock(pool->m_mutex);

    while (pool->m_task_queue.empty()) {
      if (pool->m_cur_count > pool->m_min_count) {
        pool->m_cur_count--;
        LOG_F(INFO, "thread: %d\t No task and exit.",
              ((unsigned)pthread_self() % 100));
        return;
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
