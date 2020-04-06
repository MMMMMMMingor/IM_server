#ifndef CLIENT_UTIL_HPP
#define CLIENT_UTIL_HPP

#include "message.pb.h"
#include <arpa/inet.h>
#include <chrono>
#include <fcntl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <loguru.hpp>
#include <string>
#include <sys/epoll.h>
#include <yaml.hpp>
//#include <zconf.h>
#include <unistd.h>
// message buffer size
#define BUF_SIZE 0xFFFF

// exit
#define EXIT "EXIT"

int setNonblock(int sockfd) {
  fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
  return 0;
}

void error(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
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

void read_message(int client_fd, im_message::Message &in_message) {
  using namespace google::protobuf::io;

  ZeroCopyInputStream *raw_input = new FileInputStream(client_fd);
  auto coded_input = new CodedInputStream(raw_input);

  uint32_t len;
  coded_input->ReadVarint32(&len);

  char *buf2 = new char[len];
  bool success = coded_input->ReadRaw(buf2, len);
  in_message.ParseFromArray(buf2, len);
  //      LOG_F(INFO, "%s", response.DebugString().c_str() );

  // TODO 未解决半包问题
  if (!success)
    LOG_F(ERROR, "read message fail");

  delete[] buf2;
  delete coded_input;
  delete raw_input;
}

/**
 * 发送数据（线程安全）
 * @param ctx
 * @param out_message
 */
void write_message(int client_fd, im_message::Message &out_message) {
  using namespace google::protobuf::io;

  ZeroCopyOutputStream *raw_output = new FileOutputStream(client_fd);

  auto coded_output = new google::protobuf::io::CodedOutputStream(raw_output);

  uint32_t len = out_message.ByteSizeLong();
  char *buf = new char[len];
  out_message.SerializeToArray(buf, len);

  coded_output->WriteVarint32(len);
  coded_output->WriteRaw(buf, len);

  delete[] buf;
  delete coded_output;
  delete raw_output;
}

/**
 * 登录
 * @param client_fd
 */
void login(const int client_fd) {
  std::string username;
  std::string password;

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

  write_message(client_fd, request);
}

/**
 * 心跳响应
 * @param socket_fd
 */
void keepalive_response(uint64_t client_fd, uint64_t session_id) {
  im_message::Message response;
  response.set_session_id(session_id);
  response.set_type(im_message::HeadType::KEEPALIVE_RESPONSE);

  write_message(client_fd, response);
}

#endif