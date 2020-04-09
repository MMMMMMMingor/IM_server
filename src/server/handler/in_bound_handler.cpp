
#include "server/channel.h"
#include "server/handler.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

/**
 * header + body 接收数据包，处理粘包、半包问题
 * @param ctx
 * @param in_message
 * @return 是否进行下一个handler
 */
void in_bound_handler(Context &ctx, im_message::Message &in_message) {
  using namespace google::protobuf::io;

  int fd = ctx.event.data.fd;

  ZeroCopyInputStream *raw_input = new FileInputStream(fd);
  auto coded_input = new CodedInputStream(raw_input);

  uint32_t len{0};
  bool success1 = coded_input->ReadVarint32(&len);

  if (!success1) {
    LOG_F(ERROR, "read error fail");
    //不让后面的session进行了
    ctx.channel->terminate();
    if (uint64_t i =
            SessionPool::get_instance()->find_session_id_by_fd(fd) != -1) {
      std::cout << "logined\n";
      SessionPool::get_instance()->remove_session(i);
    }
    // 如果服务器异常关闭， 则删除直接算 logout
    return;
  }

  char *buf = new char[len];
  bool success;
  try {
    success = coded_input->ReadRaw(buf, len);
  } catch (...) {
    std::cout << "catch2\n";
    return;
  }
  in_message.ParseFromArray(buf, len);
  //      LOG_F(INFO, "%s", response.DebugString().c_str() );

  // TODO 未解决半包问题
  if (!success)
    LOG_F(ERROR, "read message fail");

  ctx.need_recv_message = false; // 读取数据完成

  delete[] buf;
  delete coded_input;
  delete raw_input;
}
