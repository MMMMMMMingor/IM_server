
#include "server/channel.h"
#include "server/handler.h"

/**
 * header + body 接收数据包，处理粘包、半包问题
 * @param ctx
 * @param in_message
 * @return 是否进行下一个handler
 */
void in_bound_handler(Context &ctx, im_message::Message &in_message) {
  Channel *channel = ctx.channel;

  bool success = in_message.ParseFromArray(channel->buff_, channel->buff_len_);

  if (!success)
    close(channel->fd_);

  channel->clear_buff();
}
