//
// Created by Mingor on 2020/4/5.
//

#include <server/common_util.hpp>
#include <server/context.hpp>
#include <server/reactor.hpp>

Reactor::Reactor(int min, int max, int size, int check, int invalid)
    : m_thread_pool(new ThreadPool(min, max, size)), m_session_pool{},
      m_keep_alive{new KeepAlive{check, invalid}} {}

Reactor::~Reactor() {
  delete m_thread_pool;
  delete m_keep_alive;
}

/**
 * 初始化 Reactor
 */
void Reactor::init() { m_keep_alive->init(&m_session_pool); }

/**
 * 事件分配器
 * @param event         事件
 * @param listen_fd     监听 socket 文件描述符
 * @param epoll_fd      epoll socket 文件描述符
 */

bool Reactor::dispatch(struct epoll_event event, int listen_fd, int epoll_fd) {
  Context ctx{event, listen_fd, epoll_fd, m_session_pool, msgs, nullptr, true, users_info};
  return m_thread_pool->add_task(
      [](Context ctx) {
        check_event_type(ctx.event.events);

        Channel channel;
        ctx.channel = &channel;

        channel.pipeline(ctx);
      },
      ctx);
}
