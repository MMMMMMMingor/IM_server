#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "common.hpp"
#include "handler.hpp"

/**
 * 检测事件类型
 * @param type 事件类型
 */
void check_event_type(uint32_t type) {
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

/**
 * 事件分配器
 * @param event         事件
 * @param listen_fd     监听 socket 文件描述符
 * @param epoll_fd      epoll socket 文件描述符
 */
void dispatcher(struct epoll_event event, int listen_fd, int epoll_fd) {

  int socket_fd = event.data.fd;

  check_event_type(event.events);

  //新用户连接
  if (socket_fd == listen_fd) {
    add_client_handler(listen_fd, epoll_fd);
  } else { // 业务处理
    im_message::Message message;
    message.ParseFromFileDescriptor(socket_fd);

    switch (message.type()) {
    case im_message::LOGIN_REQUEST:
      break;
    case im_message::LOGIN_RESPONSE:
      break;
    case im_message::LOGOUT_REQUEST:
      client_logout_handler(socket_fd);
      break;
    case im_message::LOGOUT_RESPONSE:
      break;
    case im_message::KEEPALIVE_REQUEST:
      break;
    case im_message::KEEPALIVE_RESPONSE:
      break;
    case im_message::MESSAGE_REQUEST:
      //处理用户发来的消息，并转发
      transmit_message_handler(socket_fd, message);
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
}

#endif