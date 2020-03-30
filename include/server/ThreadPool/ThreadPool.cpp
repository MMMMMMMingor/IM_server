//
// Created by Firefly on 2020/3/30.
//

#include <pthread.h>
#include <iostream>
#include <loguru.hpp>
#include "ThreadPool.h"


//构造函数,确定线程池的线程数
ThreadPool::ThreadPool(size_t cntThread, size_t maxCount) {
    //初始化条件变量与互斥量
    pthread_mutex_init(&lock, nullptr);
    pthread_cond_init(&empty, nullptr);

    this->maxCount = maxCount;

    //创建线程
    std::cout << "main pthread pid is " << ((unsigned) pthread_self() % 100) << std::endl;

    pthread_t pid[cntThread];
    for (int i = 0; i < cntThread; i++) {
        // 第一个  就是 控制这个线程 句柄上面的数组
        // 第二个 是设置新的线程的，栈大小等等信息，默认为空就可以了
        // 第3个参数是一个函数指针，它返回void * 参数也是void *, 回调函数
        // 第四个 参数是线程的参数
        int ret = pthread_create(pid + i, NULL, start, this);
    }
}

ThreadPool::~ThreadPool() {
    pthread_cond_destroy(&empty);
    pthread_mutex_destroy(&lock);
    LOG_F(INFO, "ThreadPool is destroyed!!");
}

bool ThreadPool::addTask(Task task,ARG arg) {
    // 首先先获取锁
    pthread_mutex_lock(&lock);
    if (taskQueue.size() >= this->maxCount) {
        LOG_F(INFO, "the task queue is full!!!,some task will be rejected");
        pthread_mutex_unlock((&lock));
        return false;  // 返回错误
    }
    this->taskQueue.push(std::make_pair(task,arg));
    pthread_cond_broadcast(&empty);
    pthread_mutex_unlock(&lock);
    return true;
}

// 静态函数， 非成员函数调用
void *ThreadPool::start(void *arg) {
    ThreadPool *pool = (ThreadPool *) arg;
    LOG_F(INFO, "\"create thread %d\"", ((unsigned) pthread_self() % 100));
    while (true) {
        pthread_mutex_lock(&pool->lock);
        while (pool->taskQueue.empty()) {
            LOG_F(INFO, "thread: %d\tNo task go to sleep!!!",((unsigned) pthread_self() % 100));
            pthread_cond_wait(&pool->empty, &pool->lock);
        }
        auto task = pool->taskQueue.front();
        pool->taskQueue.pop();
        pthread_mutex_unlock(&pool->lock);
        task.first(task.second.event,task.second.listen_fd,task.second.epoll_fd);
    }
}
