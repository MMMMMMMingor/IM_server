#include <server/handler.h>

/**
 * 检查是否登录
 * @param ctx
 * @param in_message
 */
void check_login_handler(Context &ctx, im_message::Message &in_message) {
  im_message::HeadType type = in_message.type();

  // 检测是否登录，未登录。并且不是登录请求
  if (type != im_message::LOGIN_REQUEST &&
      !ctx.session_pool.contains(in_message.session_id())) {

    // TODO 重新设计 session_id， 这里是有bug的！
    notification_one(ctx, in_message.session_id(),
                     std::string{"您未登录，请先登录."});
    ctx.channel->terminate();

    return;
  }
}
