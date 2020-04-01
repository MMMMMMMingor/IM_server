#ifndef COMMON_HPP
#define COMMON_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <list>
#include <map>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include "server/session.hpp"
#include <time.h>
#include <server/histroy_message.hpp>

//// 作为全局变量， 不在server了
history_message *history_msg = new history_message(10);

// 上下文
struct Context {
  epoll_event event;
  int listen_fd;
  int epoll_fd;
  SessionPool &session_pool;
  Context(epoll_event event, int listen_fd, int epoll_fd,
          SessionPool &session_pool)
      : event(event), listen_fd(listen_fd),
        epoll_fd(epoll_fd), session_pool{session_pool} {}
};

// clients_list save all the clients's socket
// std::list<int> clients_list;

std::string getTime() {
  time_t timep;
  time(&timep);
  char tmp[64];
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
  return tmp; //自动转型
}

// message buffer size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME "欢迎来到文明——聊天室! 您的 chat ID : Client #%d"

// exit
#define EXIT "EXIT"

#define CAUTION "聊天室仅有您一个人。。。"

/****** some function *****/
/**
 *设置非阻塞
 */
int setNonblock(int sockfd) {
  fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
  return 0;
}

/**
 * 输出错误
 * @param msg 错误信息
 */
void error(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

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

#endif // COMMON_HPP