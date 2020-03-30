//
// Created by Firefly on 2020/3/30.
//

#ifndef IM_SERVER_THREADPOOL_H
#define IM_SERVER_THREADPOOL_H

#include <cstddef>
#include <sys/types.h>
#include <queue>
#include <sys/epoll.h>

typedef void (*Task)(epoll_event, int, int);

struct ARG {
    epoll_event event;
    int listen_fd;
    int epoll_fd;
    ARG(epoll_event e, int lf, int ef) : event(e), listen_fd(lf), epoll_fd(ef) {}
};


class ThreadPool {
private:
    //定义任务队列
    std::queue<std::pair<Task, ARG>> taskQueue;
    //最大任务数量
    size_t maxCount;
    //当前任务数量
    size_t curCount;
    //条件变量
    pthread_cond_t empty;
    //互斥量
    pthread_mutex_t lock;

    //线程入口
    static void *start(void *);

public:

    //构造函数,确定线程池的线程数
    ThreadPool(size_t cntThread, size_t maxCount);

    //析构函数
    ~ThreadPool();

    //添加任务
    bool addTask(Task, ARG);
};


#endif //IM_SERVER_THREADPOOL_H
