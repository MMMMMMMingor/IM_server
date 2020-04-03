/*
 * @Author: Firefly
 * @Date: 2020-03-31 23:48:24
 * @Descripttion: 
 * @LastEditTime: 2020-04-02 12:33:54
 */
#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "loguru.hpp"
#include "message.pb.h"
#include "server/session.hpp"
#include <string>
#include <utility>

/**
 * 广播消息
 * @param Context       上下文
 * @param message
 */
void board_cast_message(Context &ctx, const im_message::Message &message) {

  auto lambda = [&message](uint64_t session_id, Session *session) {
    bool success = message.SerializeToFileDescriptor(session->get_socket_fd());

    if (!success) {
      
      LOG_F(ERROR, "board cast error");
      
      //      exit(EXIT_FAILURE);
    }
  };
  LOG_F(INFO, "--------------board_cast_message-------------------");

  ctx.session_pool.for_each(lambda);
}

void board_cast_message_by_room_id(Context &ctx, const im_message::Message &message,std::string room_id) {

    auto lambda = [&message,&room_id](uint64_t session_id, Session *session) {
        if(room_id != session->get_room_name()) return; // 相同的 roomid 才执行发送
        bool success = message.SerializeToFileDescriptor(session->get_socket_fd());
        if (!success) {

            LOG_F(ERROR, "board cast error");

            //      exit(EXIT_FAILURE);
        }
    };
    LOG_F(INFO, "--------------board_cast_message_by_room_id-------------------");

    ctx.session_pool.for_each(lambda);
}

/**
 * 通知某一个房间的所有用户
 * @param ctx
 * @param message
 */
void notification_by_room_id(Context &ctx, const std::string &message,std::string room_id) {
    im_message::Message response;
    response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
    auto *message_notification = new im_message::MessageNotification;

    message_notification->set_json(message);
    message_notification->set_timestamp(getTime());
    response.set_allocated_notification(message_notification);

    board_cast_message_by_room_id(ctx, response,std::move(room_id));
}


/**
 * 通知所有用户
 * @param Context       上下文
 * @param message       消息
 */
void notification_all(Context &ctx, const std::string &message) {
  im_message::Message response;
  response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
  auto *message_notification = new im_message::MessageNotification;
  //! 在哪里 del？？？？？？？？？？？？？？？？？？？
  message_notification->set_json(message);
  message_notification->set_timestamp(getTime());
  response.set_allocated_notification(message_notification);

  board_cast_message(ctx, response);
}

/**
 * 消息通知单个用户
 * @param Context       上下文
 * @param message       消息
 */
void notification_one(Context &ctx, const std::string &message) {
  int client_fd = ctx.event.data.fd;

  im_message::Message response;
  response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
  auto *message_notification = new im_message::MessageNotification;
  message_notification->set_json(message);
  response.set_allocated_notification(message_notification);

  response.SerializeToFileDescriptor(client_fd);
}

#endif // !HANDLER_HPP
