#include <loguru.hpp>
#include <message.pb.h>
#include <sys/socket.h>
#include <unistd.h>

#define FILE_MODE

#ifdef FILE_MODE

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
bool recv_message(int fd, im_message::Message &message) {
  size_t len;
  read_until(fd, &len, sizeof(size_t));

  void *data = malloc(len);
  read_until(fd, data, len);

  //  LOG_F(INFO, "read %zu", len);
  bool success = message.ParseFromArray(data, len);
  free(data);

  return success;
}

/**
 * header + body 发送数据包，处理粘包、半包问题
 * @param fd
 * @param message
 */
bool send_message(int fd, const im_message::Message &message) {

  size_t len = message.ByteSizeLong();
  write(fd, &len, sizeof(len));

  return message.SerializeToFileDescriptor(fd);
}

#else

inline bool recv_until(int fd, void *ptr, size_t len, int flag) {

  char *tmp_ptr = static_cast<char *>(ptr);
  size_t left_len = len;
  while (left_len) {
    int tmp_len = recv(fd, tmp_ptr + (len - left_len), left_len, flag);
    if (tmp_len < 0)
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
void recv_message(int fd, im_message::Message &message) {
  size_t len;
  recv_until(fd, &len, sizeof(size_t), 0);

  void *data = malloc(len);
  recv_until(fd, data, len, 0);

  LOG_F(INFO, "read size %zu", len);
  message.ParseFromArray(data, len);
  free(data);
  LOG_F(INFO, "%s", message.DebugString().c_str());
}

/**
 * header + body 发送数据包，处理粘包、半包问题
 * @param fd
 * @param message
 */
bool send_message(int fd, const im_message::Message &message) {

  size_t len = message.ByteSizeLong();
  send(fd, &len, sizeof(len), 0);

  LOG_F(INFO, "write len %zi ", len);

  return message.SerializeToFileDescriptor(fd);

  //  void *data = malloc(len);
  //  message.SerializeToArray(data, len);
  //  //  write(fd, data, len);
  //  send(fd, data, len, 0);
  //  free(data);
}
#endif