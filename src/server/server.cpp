#include "common.hpp"
#include "loguru.hpp"
#include "server/reactor.hpp"
#include <memory>
#include <yaml.hpp>

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
class Server {
private:
  int listener{};
  int epfd{};
  struct sockaddr_in serverAddr {};
  struct epoll_event *events{};
  std::string SERVER_IP;
  uint16_t SERVER_PORT{};
  int EPOLL_SIZE{};

  int THREAD_MIN_NUM{};
  int THREAD_MAX_NUM{};
  int THREAD_QUEUE_SIZE{};
  int KEEPALIVE_CHECK_INTERVAL{};
  int KEEPALIVE_INVALID_INTERVAL{};

  Reactor *reactor{};



public:
  Server() { init(); }

  ~Server() {
    delete reactor;
    close(listener); //关闭socket
    close(epfd);     //关闭内核
  }

  void init() {
    configuration(); //顺序不可更改
    init_epoll();
    init_reactor();
  }

  void init_reactor() {
    reactor = new Reactor(THREAD_MIN_NUM, THREAD_MAX_NUM, THREAD_QUEUE_SIZE,
                          KEEPALIVE_CHECK_INTERVAL, KEEPALIVE_INVALID_INTERVAL);
    reactor->init();
  }

  void init_epoll() {
    /**
     * 1:创建套接字socket
     * param1:指定地址族为IPv4;param2:指定传输协议为流式套接字;param3:指定传输协议为TCP,可设为0,由系统推导
     */
    this->listener = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->listener < 0) {
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

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());

    //绑定地址
    if (bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
        0) {
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
    epfd = epoll_create(EPOLL_SIZE);
    if (epfd < 0) {
      LOG_F(ERROR, "epfd create error");
      exit(EXIT_FAILURE);
    }
    LOG_F(INFO, "epoll created, epollfd = %d", epfd);

    events = new epoll_event[EPOLL_SIZE];
    //往内核事件表里添加事件
    add_fd(epfd, listener, true);
  }

  void configuration() {
    Yaml::Node root;
    Yaml::Parse(root, "../config/server.yml");
    this->SERVER_IP = root["server"]["ip"].As<std::string>();
    this->SERVER_PORT = root["server"]["port"].As<uint16_t>();
    this->EPOLL_SIZE = root["server"]["epoll"]["size"].As<int>();
    this->THREAD_MIN_NUM = root["server"]["threadPool"]["min"].As<int>();
    this->THREAD_MAX_NUM = root["server"]["threadPool"]["max"].As<int>();
    this->THREAD_QUEUE_SIZE =
        root["server"]["threadPool"]["queueSize"].As<int>();
    this->KEEPALIVE_CHECK_INTERVAL =
        root["server"]["keepalive"]["check_interval"].As<int>();
    this->KEEPALIVE_INVALID_INTERVAL =
        root["server"]["keepalive"]["invalid_interval"].As<int>();
  }

  void start() {
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
        // 事件分配器到线程池
        while (!(reactor->dispatch(events[i], listener, epfd))) {
          LOG_F(INFO, "任务队列已满，服务器宕机了");
          sleep(10);
        }
        // dispatcher(events[i], listener, epfd);
      }
    }
  }
};

int main(int argc, char *argv[]) {

  // 初始化 loguru 日志库 // 属于全局的？
  loguru::init(argc, argv);

  Server server;
  server.start();

  return 0;
}