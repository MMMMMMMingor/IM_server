/*
 * @Author: Firefly
 * @Date: 2020-03-31 23:48:24
 * @Descripttion:
 * @LastEditTime: 2020-04-02 12:33:54
 */
#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include "session.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <loguru.hpp>
#include <message.pb.h>
#include <server/context.hpp>

/**
 * 获取当前时间戳
 * @return
 */
std::string getTime();

/**
 *设置非阻塞
 */
int setNonblock(int sockfd);

/**
 * 将文件描述符 fd 添加到 epollfd 标示的内核事件表中,
 * 并注册 EPOLLIN 和 EPOOLET 事件,
 * EPOLLIN 是数据可读事件；EPOOLET 表明是 ET 工作方式。
 * 最后将文件描述符设置非阻塞方式
 * @param epoll_fd :epoll 文件描述符
 * @param fd      :文件描述符
 * @param enable_et:enable_et = true,
 * 是否采用epoll的ET(边缘触发)工作方式；否则采用LT(水平触发)工作方式
 */
void add_fd(int epollfd, int fd, bool enable_et);

/**
 * 发送数据（线程安全）
 * @param ctx
 * @param out_message
 */
void send_message_safe(Session *session,
                       const im_message::Message &out_message);

/**
 * 发送数据（线程安全）
 * @param ctx
 * @param out_message
 */
void send_message(Context &ctx, const im_message::Message &out_message);
/**
 * 广播消息
 * @param Context       上下文
 * @param message
 */
void board_cast_message(Context &ctx, const im_message::Message &message);

void board_cast_message_by_room_id(Context &ctx,
                                   const im_message::Message &message,
                                   const std::string &room_id);

/**
 * 通知某一个房间的所有用户
 * @param ctx
 * @param message
 */
void notification_by_room_id(Context &ctx, const std::string &message,
                             const std::string &room_id);

/**
 * 通知所有用户
 * @param Context       上下文
 * @param message       消息
 */
void notification_all(Context &ctx, const std::string &message);

/**
 * 消息通知单个用户
 * @param Context       上下文
 * @param message       消息
 */
void notification_one(Context &ctx, uint64_t session_id,
                      const std::string &message);

/**
 * 检测事件类型
 * @param type 事件类型
 */
void check_event_type(uint32_t type);

#endif // !COMMON_UTIL_H
