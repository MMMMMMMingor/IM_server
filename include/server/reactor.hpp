#ifndef REACTOR_HPP
#define REACTOR_HPP

#include "handler.hpp"
#include "session.hpp"
#include "threadpool.hpp"
#include <memory>

/**
 * 检测事件类型
 * @param type 事件类型
 */
inline void check_event_type(uint32_t type) {
  if (type & EPOLLIN)
    LOG_F(INFO, "type check:\tEPOLLIN");
  if (type & EPOLLOUT)
    LOG_F(INFO, "type check:\tEPOLLOUT");
  if (type & EPOLLRDHUP)
    LOG_F(INFO, "type check:\tEPOLLRDHUP ");
  if (type & EPOLLPRI)
    LOG_F(INFO, "type check:\tEPOLLPRI");
  if (type & EPOLLERR)
    LOG_F(INFO, "type check:\tEPOLLERR");
  if (type & EPOLLHUP)
    LOG_F(INFO, "type check:\tEPOLLHUP");
  if (type & EPOLLET)
    LOG_F(INFO, "type check:\tEPOLLET");
  if (type & EPOLLONESHOT)
    LOG_F(INFO, "type check:\tEPOLLONESHOT ");
  if (type & EPOLLWAKEUP)
    LOG_F(INFO, "type check:\tEPOLLWAKEUP ");
}

class Reactor {

public:
  Reactor(int min, int max, int size)
      : m_thread_pool(new ThreadPool(min, max, size)), m_session_pool{} {}

  ~Reactor() { delete m_thread_pool; }

  /**
   * 事件分配器
   * @param event         事件
   * @param listen_fd     监听 socket 文件描述符
   * @param epoll_fd      epoll socket 文件描述符
   */
  bool dispatch(struct epoll_event event, int listen_fd, int epoll_fd) {
    Context ctx{event, listen_fd, epoll_fd, m_session_pool};
    return m_thread_pool->add_task(
        [](Context ctx) {
          int socket_fd = ctx.event.data.fd;

          check_event_type(ctx.event.events);

          //新用户连接
          if (socket_fd == ctx.listen_fd) {
            create_session_handler(ctx);
          } else { // 业务处理
            im_message::Message message;
            message.ParseFromFileDescriptor(socket_fd);

            switch (message.type()) {
            case im_message::LOGIN_REQUEST:
              client_login_handler(ctx, message);
              break;
            case im_message::LOGIN_RESPONSE:
              break;
            case im_message::LOGOUT_REQUEST:
              client_logout_handler(ctx, message);
              break;
            case im_message::LOGOUT_RESPONSE:
              break;
            case im_message::KEEPALIVE_REQUEST:
              break;
            case im_message::KEEPALIVE_RESPONSE:
              break;
            case im_message::MESSAGE_REQUEST:
              //处理用户发来的消息，并转发
              transmit_message_handler(ctx, message);
              break;
            case im_message::MESSAGE_RESPONSE:
              break;
            case im_message::MESSAGE_NOTIFICATION:
              break;
            case im_message::HeadType_INT_MIN_SENTINEL_DO_NOT_USE_:
              break;
            case im_message::HeadType_INT_MAX_SENTINEL_DO_NOT_USE_:
              break;
            }
          }
        },
        ctx);
  }

private:
  ThreadPool *m_thread_pool;
  SessionPool m_session_pool;
};

#endif // REACTOR_HPP