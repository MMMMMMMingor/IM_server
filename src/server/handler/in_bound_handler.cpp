
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <server/handler.h>

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

    uint32_t len;
    bool success1 = coded_input->ReadVarint32(&len);

    if (!success1) {
        LOG_F(ERROR, "read error fail");
        unsigned long long session_id = ctx.session_pool.find_session_id_by_fd(ctx.epoll_fd);
        std::cout << session_id << std::endl;
        if(session_id != -1) ctx.session_pool.remove_session(session_id);
        return;
    }

    char *buf = new char[len];
    bool success = coded_input->ReadRaw(buf, len);
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
//
// inline bool read_until(int fd, void *ptr, size_t len) {
//  char *tmp_ptr = static_cast<char *>(ptr);
//  size_t left_len = len;
//  while (left_len) {
//    //    LOG_F(INFO, "left %zu", len);
//    int tmp_len = read(fd, tmp_ptr + (len - left_len), left_len);
//    if (tmp_len <= 0)
//      return false;
//    left_len -= tmp_len;
//  }
//
//  return true;
//}

// bool in_bound_handler(Context ctx, im_message::Message &in_message,
//                      im_message::Message &out_message) {
//  int fd = ctx.event.data.fd;
//
//  size_t len;
//  read_until(fd, &len, sizeof(size_t));
//
//  void *data = malloc(len);
//  read_until(fd, data, len);
//
//  //  LOG_F(INFO, "read %zu", len);
//  bool success = in_message.ParseFromArray(data, len);
//  free(data);
//
//  return success;
//}
