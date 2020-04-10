//
// Created by Mingor on 2020/4/5.
//
#include "server/common_util.hpp"
#include "common.h"
#include "server/websocket_protocol.h"

std::string getTime() {
  time_t time_stamp;
  time(&time_stamp);
  char tmp[64];
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&time_stamp));
  return tmp; //自动转型
}

int setNonblock(int sockfd) {
  fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
  return 0;
}

void add_fd(int epollfd, int fd, bool enable_et) {
  struct epoll_event ev {};
  ev.data.fd = fd;
  ev.events = EPOLLIN;
  if (enable_et) {
    ev.events = EPOLLIN | EPOLLET;
  }
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
  setNonblock(fd);
}

void send_message_safe(Session *session,
                       const im_message::Message &out_message) {
  using namespace google::protobuf::io;
  LOG_F(INFO, "--------------send_message_safe-------------------");

  std::lock_guard<std::mutex> lg(session->get_mutex());
  int fd = session->get_socket_fd();

  std::string out_frame;
  WebSocket_Protocol::ws_encode_frame(
      out_message.SerializeAsString(), out_frame,
      WebSocket_Protocol::WS_FrameType::WS_TEXT_FRAME);

  write(fd, out_frame.data(), out_frame.length());
}

void send_message(Context &ctx, const im_message::Message &out_message) {
  LOG_F(INFO, "--------------send_message-------------------");
  Session *session =
      SessionPool::get_instance()->find_session(out_message.session_id());

  send_message_safe(session, out_message);
}

void board_cast_message(Context &ctx, const im_message::Message &message) {
  LOG_F(INFO, "--------------board_cast_message-------------------");

  auto lambda = [&message](uint64_t session_id, Session *session) {
    send_message_safe(session, message);
  };

  SessionPool::get_instance()->for_each(lambda);
}

void board_cast_message_by_room_id(Context &ctx,
                                   const im_message::Message &message,
                                   const std::string &room_id) {

  LOG_F(INFO, "--------------board_cast_message_by_room_id-------------------");

  auto lambda = [&ctx, &message, &room_id](uint64_t session_id,
                                           Session *session) {
    if (room_id != session->get_room_name())
      return; // 相同的 room id 才执行发送

    ctx.msg_map[room_id]->add_one(message); //只存储播报的消息

    send_message_safe(session, message);
  };

  SessionPool::get_instance()->for_each(lambda);
}

void notification_by_room_id(Context &ctx, const std::string &message,
                             const std::string &room_id) {
  LOG_F(INFO, "--------------notification_by_room_id-------------------");

  im_message::Message response;
  response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
  auto *message_notification = new im_message::MessageNotification;

  message_notification->set_json(message);
  message_notification->set_timestamp(getTime());
  response.set_allocated_notification(message_notification);

  board_cast_message_by_room_id(ctx, response, room_id);
}

void notification_all(Context &ctx, const std::string &message) {
  LOG_F(INFO, "--------------notification_all-------------------");

  im_message::Message response;
  response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
  auto *message_notification = new im_message::MessageNotification;
  //! 在哪里 del？？？？？？？？？？？？？？？？？？？
  message_notification->set_json(message);
  message_notification->set_timestamp(getTime());
  response.set_allocated_notification(message_notification);

  board_cast_message(ctx, response);
}

void notification_one(Context &ctx, uint64_t session_id,
                      const std::string &message) {
  LOG_F(INFO, "--------------notification_one-------------------");

  im_message::Message response;
  response.set_session_id(session_id);
  response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
  auto *message_notification = new im_message::MessageNotification;
  message_notification->set_json(message);
  response.set_allocated_notification(message_notification);

  send_message(ctx, response);
}

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
