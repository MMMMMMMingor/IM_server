#ifndef OUT_BOUND_HANDLER
#define OUT_BOUND_HANDLER

/**
 * header + body 发送数据包，处理粘包、半包问题
 * @param fd
 * @param message
 */
bool out_bound_handler(int fd, const im_message::Message &message) {

  size_t len = message.ByteSizeLong();
  write(fd, &len, sizeof(len));

  return message.SerializeToFileDescriptor(fd);
}

#endif