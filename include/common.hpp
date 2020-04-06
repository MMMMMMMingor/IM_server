/*
 * @Author: Firefly
 * @Date: 2020-03-31 21:42:07
 * @Descripttion:
 * @LastEditTime: 2020-04-01 23:36:44
 */
#ifndef COMMON_HPP
#define COMMON_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <server/channel.h>
#include <server/histroy_message.h>
#include <server/session.h>
#include <sys/epoll.h>
#include <unordered_map>
//#include <zconf.h>
#include <unistd.h>
///**
// * 获取当前时间戳
// * @return
// */
// std::string getTime() {
//  time_t time_stamp;
//  time(&time_stamp);
//  char tmp[64];
//  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&time_stamp));
//  return tmp; //自动转型
//}
//
///****** some function *****/
///**
// *设置非阻塞
// */
// int setNonblock(int sockfd) {
//  fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
//  return 0;
//}
//
///**
// * 将文件描述符 fd 添加到 epollfd 标示的内核事件表中,
// * 并注册 EPOLLIN 和 EPOOLET 事件,
// * EPOLLIN 是数据可读事件；EPOOLET 表明是 ET 工作方式。
// * 最后将文件描述符设置非阻塞方式
// * @param epoll_fd :epoll 文件描述符
// * @param fd      :文件描述符
// * @param enable_et:enable_et = true,
// * 是否采用epoll的ET(边缘触发)工作方式；否则采用LT(水平触发)工作方式
// */
// void add_fd(int epollfd, int fd, bool enable_et) {
//  struct epoll_event ev {};
//  ev.data.fd = fd;
//  ev.events = EPOLLIN;
//  if (enable_et) {
//    ev.events = EPOLLIN | EPOLLET;
//  }
//  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
//  setNonblock(fd);
//}

#endif // COMMON_HPP