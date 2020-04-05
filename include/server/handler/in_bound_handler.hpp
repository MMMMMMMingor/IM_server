#ifndef IN_BOUND_HANDLER_HPP
#define IN_BOUND_HANDLER_HPP

#include <loguru.hpp>
#include <message.pb.h>
#include <sys/socket.h>
#include <unistd.h>

inline bool read_until(int fd, void *ptr, size_t len) {
  char *tmp_ptr = static_cast<char *>(ptr);
  size_t left_len = len;
  while (left_len) {
//    LOG_F(INFO, "left %zu", len);
    int tmp_len = read(fd, tmp_ptr + (len - left_len), left_len);
    if (tmp_len <= 0)
      return false;
    left_len -= tmp_len;
  }

  return true;
}

/**
 * header + body 接收数据包，处理粘包、半包问题
 * @param fd
 * @param message
 */
bool in_bound_handler(int fd, im_message::Message &message) {
  size_t len;
  read_until(fd, &len, sizeof(size_t));

  void *data = malloc(len);
  read_until(fd, data, len);

  //  LOG_F(INFO, "read %zu", len);
  bool success = message.ParseFromArray(data, len);
  free(data);

  return success;
}

#endif