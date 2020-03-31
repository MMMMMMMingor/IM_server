#ifndef CREATE_SESSION_HANDLER_HPP
#define CREATE_SESSION_HANDLER_HPP
/**
 * 创建session会话
 * @param Context       上下文
 */
void create_session_handler(Context ctx) {
  struct sockaddr_in client_address {};
  socklen_t client_addrLength = sizeof(struct sockaddr_in);
  int client_fd = accept(ctx.listen_fd, (struct sockaddr *)&client_address,
                         &client_addrLength);

  LOG_F(INFO, "client connection from: %s : % d(IP : port), client_fd = %d ",
        inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port),
        client_fd);

  add_fd(ctx.epoll_fd, client_fd, true);
  LOG_F(INFO, "Add new client_fd = %d to epoll", client_fd);
}
#endif