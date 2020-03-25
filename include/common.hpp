#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>

#include <time.h>

using namespace std;


struct USER {
    string addr;
    int port;
    string inTime;
    USER(string a, int p, string i) { addr = a, port = p, inTime = i; }
};

struct MSG {
    int num;        // 第几条消息， 到一定数量自动删除
    string time;    // 消息时间
    USER *user;     // 指向user， 空则为下线
    string info;    // 发来的二进制消息
};

// clients_list save all the clients's socket
list<int> clients_list;

map<int, USER *> users;  // 在线用户的信息, 通过 client_list 找

list<MSG> msg;              // 存储所有的信息





string getTime() {
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return tmp;  //自动转型
}




/***** macro defintion *****/
// server ip
#define SERVER_IP "127.0.0.1"

// server port
#define SERVER_PORT 8888

// epoll size
#define EPOLL_SIZE 5000

// message buffer size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME                                                         \
  "Welcome you join to the chat room! Your chat ID is: Client #%d"

#define SERVER_MESSAGE "ClientID %d say >> %s"

// exit
#define EXIT "EXIT"

#define CAUTION "There is only ont int the char root!"






/****** some function *****/
/**
 *设置非阻塞
 */
int setNonblock(int sockfd) {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

// 输出错误
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

/**
 * 将文件描述符 fd 添加到 epollfd 标示的内核事件表中,
 * 并注册 EPOLLIN 和 EPOOLET 事件,
 * EPOLLIN 是数据可读事件；EPOOLET 表明是 ET 工作方式。
 * 最后将文件描述符设置非阻塞方式
 * @param epollfd:epoll句柄
 * @param fd:文件描述符
 * @param enable_et:enable_et = true,
 * 是否采用epoll的ET(边缘触发)工作方式；否则采用LT(水平触发)工作方式
 */
void add_fd(int epollfd, int fd, bool enable_et) {
    struct epoll_event ev{};
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et) {
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setNonblock(fd);
}

#endif // COMMON_H