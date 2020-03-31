#include "common.hpp"
#include "message.pb.h"
#include <iostream>
#include <string>
#include <yaml.hpp>

static std::string username;
static std::string password;
static uint64_t session_id;
static std::string SERVER_IP;
static uint16_t SERVER_PORT;

/**
 * 初始化服务器参数
 */
void client_configuration() {
  Yaml::Node root;
  Yaml::Parse(root, "../config/client.yml");

  SERVER_IP = root["server"]["ip"].As<std::string>();
  SERVER_PORT = root["server"]["port"].As<uint16_t>();
}

/**
 * 登录
 * @param client_fd
 */
void login(const int client_fd) {

  std::cout << "用户名：";
  std::cin >> username;
  std::cout << "密码：";
  std::cin >> password;

  im_message::Message request;
  request.set_type(im_message::HeadType::LOGIN_REQUEST);
  auto *login_request = new im_message::LoginRequest;
  login_request->set_username(username);
  login_request->set_password(password);
  request.set_allocated_loginrequest(login_request);
  request.SerializeToFileDescriptor(client_fd);
}

int main(int argc, char *argv[]) {
  /**
   * TCP 客户端通信
   * 1.创建套接字（socket）
   * 2.使用 connect() 建立到达服务器的连接（connect)
   * 3.客户端进行通信（使用 write()/send() 或 send()/recv() )
   * 4.使用 close() 关闭客户连接
   */

  /**
   * 1:创建套接字socket
   * param1:指定地址族为IPv4;param2:指定传输协议为流式套接字;param3:指定传输协议为TCP,可设为0,由系统推导
   */
  client_configuration();

  int client_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_fd < 0) {
    error("socket error");
  }

  // 填充sockadd结构,指定ip与端口
  struct sockaddr_in serverAddr {};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(SERVER_PORT);
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());

  // 2:连接服务端
  if (connect(client_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    error("connect error");
  }

  // 创建管道，其中fd[0]用于父进程读，fd[1]用于子进程写
  int pipefd[2];
  if (pipe(pipefd) < 0) {
    error("pipe error");
  }

  /**
   * epoll使用
   * 1:调用 epoll_create 函数在 Linux 内核中创建一个事件表；
   * 2:然后将文件描述符添加到所创建的事件表中 (epoll_ctl)；
   * 3:在主循环中，调用 epoll_wait 等待返回就绪的文件描述符集合；
   * 4:分别处理就绪的事件集合
   */
  // 创建epoll
  int epfd = epoll_create(2);
  if (epfd < 0) {
    error("epfd error");
  }

  static struct epoll_event events[2];
  //将sock和管道读端描述符都添加到内核事件表中
  add_fd(epfd, client_fd, true);
  add_fd(epfd, pipefd[0], true);

  // 表示客户端是否正常工作
  bool isClientwork = true;

  // 聊天信息缓冲区
  char message[BUF_SIZE];

  login(client_fd);

  // Fork
  int pid = fork();
  if (pid < 0) {
    error("fork error");
  } else if (pid == 0) { // 子进程
    //子进程负责写入管道，因此先关闭读端
    close(pipefd[0]);
    printf("Please input 'exit' to exit the IM_server room\n");

    while (isClientwork) {
      bzero(&message, BUF_SIZE);

      fgets(message, BUF_SIZE, stdin);

      // 客户输出exit,退出
      if (strncasecmp(message, EXIT, strlen(EXIT)) == 0) {
        isClientwork = false;
        write(pipefd[1], message, 0);
      } else { // 子进程将信息写入管道
        if (write(pipefd[1], message, strlen(message) - 1) < 0) {
          error("fork error");
        }
      }
    }
  } else { // pid > 0 父进程
    //父进程负责读管道数据，因此先关闭写端
    close(pipefd[1]);

    // 主循环(epoll_wait)
    while (isClientwork) {
      // 等待事件的产生，函数返回需要处理的事件数目
      int epoll_events_count = epoll_wait(epfd, events, 2, -1);
      // 处理就绪事件
      for (int i = 0; i < epoll_events_count; ++i) {
        bzero(&message, BUF_SIZE);

        //服务端发来消息
        if (events[i].data.fd == client_fd) {
          //接受服务端消息

          im_message::Message response;
          response.ParseFromFileDescriptor(client_fd);

          switch (response.type()) {
          case im_message::HeadType::MESSAGE_RESPONSE: {
            printf("%s\n", response.messageresponse().info().c_str());
          }
          case im_message::HeadType::MESSAGE_NOTIFICATION: {
            printf("%s %s\n", response.notification().timestamp().c_str(),
                   response.notification().json().c_str());
          }
          case im_message::LOGIN_REQUEST:
            break;
          case im_message::LOGIN_RESPONSE:
            session_id = response.session_id();
            printf("%s\n", response.loginresponse().info().c_str());
            break;
          case im_message::LOGOUT_REQUEST:
            break;
          case im_message::LOGOUT_RESPONSE: {
            isClientwork = false;
            break;
          }
          case im_message::KEEPALIVE_REQUEST:
            break;
          case im_message::KEEPALIVE_RESPONSE:
            break;
          case im_message::MESSAGE_REQUEST:
            printf("%s -> %s\n", response.messagerequest().from_nick().c_str(),
                   response.messagerequest().content().c_str());
            break;
          case im_message::HeadType_INT_MIN_SENTINEL_DO_NOT_USE_:
            break;
          case im_message::HeadType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
          }

        }
        //子进程写入事件发生，父进程处理并发送服务端
        else {
          //父进程从管道中读取数据
          int ret = read(events[i].data.fd, message, BUF_SIZE);
          if (ret == 0) {

            im_message::Message request;
            request.set_type(im_message::HeadType::LOGOUT_REQUEST);

            request.SerializeToFileDescriptor(client_fd);
            printf("logout\n");
            continue;
          }
          im_message::Message request;
          request.set_type(im_message::HeadType::MESSAGE_REQUEST);
          request.set_session_id(session_id);
          auto *message_request = new im_message::MessageRequest{};
          message_request->set_content(message);
          request.set_allocated_messagerequest(message_request);

          bool success = request.SerializeToFileDescriptor(client_fd);

          // ret = 0
          if (!success) {
            isClientwork = false;
          }
        }
      } // for
    }   // while
  }

  if (pid) {
    //关闭父进程和sock
    close(pipefd[0]);
    close(client_fd);
  } else {
    //关闭子进程
    close(pipefd[1]);
  }

  return 0;
}