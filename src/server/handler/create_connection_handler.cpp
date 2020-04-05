#include <common.hpp>
#include <server/handler.h>

/**
 * 创建新的连接
 * @param Context       上下文
 */
void create_connection_handler(Context &ctx, im_message::Message &in_message) {
  if (ctx.event.data.fd != ctx.listen_fd) {
    return;
  }

  struct sockaddr_in client_address {};
  socklen_t client_addrLength = sizeof(struct sockaddr_in);
  int client_fd = accept(ctx.listen_fd, (struct sockaddr *)&client_address,
                         &client_addrLength);

  LOG_F(INFO, "用户连接: %s : % d(IP : port), client_fd = %d ",
        inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port),
        client_fd);

  add_fd(ctx.epoll_fd, client_fd, true);
  LOG_F(INFO, "Add new client_fd = %d to epoll", client_fd);

  ctx.channel->terminate();
}
