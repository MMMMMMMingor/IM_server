#include "common.hpp"
#include "loguru.hpp"
#include "server/dispatcher.hpp"
#include <yaml.hpp>

static std::string SERVER_IP;
static uint16_t SERVER_PORT;
static int EPOLL_SIZE;

/**
 * 初始化服务器参数
 */
void configuration() {
  Yaml::Node root;
  Yaml::Parse(root, "../config/server.yml");

  SERVER_IP = root["server"]["ip"].As<std::string>();
  SERVER_PORT = root["server"]["port"].As<uint16_t>();
  EPOLL_SIZE = root["server"]["epoll"]["size"].As<int>();
}

int main(int argc, char *argv[]) {
  /**
   * TCP服务端通信
   * 1：使用 socket()创建 TCP 套接字（socket）
   * 2：将创建的套接字绑定到一个本地地址和端口上（bind）
   * 3：将套接字设为监听模式，准备接收客户端请求（listen）
   * 4：等待客户请求到来:
   * 当请求到来后，接受连接请求，返回一个对应于此次连接的新的套接字（accept）
   * 5：用 accept
   * 返回的套接字和客户端进行通信（使用write()/send()或send()/recv())
   * 6：返回，等待另一个客户请求
   * 7：关闭套接字
   */

  // 初始化 loguru 日志库
  loguru::init(argc, argv);

  configuration();
  /**
   * 1:创建套接字socket
   * param1:指定地址族为IPv4;param2:指定传输协议为流式套接字;param3:指定传输协议为TCP,可设为0,由系统推导
   */
  int listener = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listener < 0) {
    LOG_F(ERROR, "socket error");
    exit(EXIT_FAILURE);
  }

  LOG_F(INFO, "listen socket created ");

  //地址复用
  int on = 1;
  if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    LOG_F(ERROR, "setsockopt error");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serverAddr {};
  serverAddr.sin_family = PF_INET;
  serverAddr.sin_port = htons(SERVER_PORT);
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());

  //绑定地址
  if (bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    LOG_F(ERROR, "bind error");
    exit(EXIT_FAILURE);
  }

  //监听
  if (listen(listener, SOMAXCONN) < 0) {
    LOG_F(ERROR, "listen error");
    exit(EXIT_FAILURE);
  }

  LOG_F(INFO, "Start to listen: %s", SERVER_IP.c_str());

  //在内核中创建事件表
  int epfd = epoll_create(EPOLL_SIZE);
  if (epfd < 0) {
    LOG_F(ERROR, "epfd create error");
    exit(EXIT_FAILURE);
  }

  LOG_F(INFO, "epoll created, epollfd = %d", epfd);
  struct epoll_event events[EPOLL_SIZE];
  //往内核事件表里添加事件
  add_fd(epfd, listener, true);

  //主循环
  while (true) {
    // epoll_events_count表示就绪事件的数目, 这里会循环等待， 直到有事件的到来
    int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
    if (epoll_events_count < 0) {
      LOG_F(ERROR, "epoll failure");
      break;
    }

    LOG_F(INFO, "epoll events count : %d", epoll_events_count);

    //处理这epoll_events_count个就绪事件
    for (int i = 0; i < epoll_events_count; ++i) {
      // 事件分配器
      dispatcher(events[i], listener, epfd);
    }
  }

  close(listener); //关闭socket
  close(epfd);     //关闭内核

  return 0;
}