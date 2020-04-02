#ifndef CLIENT_LOGIN_HANDLER_HPP
#define CLIENT_LOGIN_HANDLER_HPP

#include "common_handler.hpp"
#include <chrono>

/**
 * 用户登录
 * @param Context       上下文
 * @param login_request 消息
 */
void client_login_handler(Context ctx, const im_message::Message &message) {
    int client_fd = ctx.event.data.fd;
    const auto &login_request = message.loginrequest();
    // 获取房间号和 用户名
    const std::string &username(login_request.username());
    const std::string &pwd = login_request.password();

    // 添加索引
    if (ctx.msg_map.find(pwd) == ctx.msg_map.end()){
        ctx.msg_map[pwd] = new history_message();
    }



    // 添加session
    auto *session = new Session{};
    session->set_username(login_request.username());
    session->set_last_keepalive(std::chrono::system_clock::now());
    session->set_socket_fd(client_fd);
    //增添 room_name
    session->set_room_name(pwd);

    uint64_t session_id = ctx.session_pool.add_session(session);


    LOG_F(INFO, "用户登录：%s : %s", username.c_str(),
          login_request.password().c_str());

    LOG_F(INFO, "Now there are %d clients int the IM_server room",
          ctx.session_pool.get_current_count());

    // 服务端发送欢迎信息
    char welcome[BUF_SIZE];
    bzero(welcome, BUF_SIZE);
    sprintf(welcome, SERVER_WELCOME, client_fd);

    im_message::Message response;
    response.set_type(im_message::HeadType::LOGIN_RESPONSE);
    response.set_session_id(session_id);
    auto *login_response = new im_message::LoginResponse;
    login_response->set_result(true);
    login_response->set_info(welcome);
    response.set_allocated_loginresponse(login_response);

    bool success = response.SerializeToFileDescriptor(client_fd);

    //  std::string login_notification = login_request.username();
    //  login_notification += " 登录";
    //  notification_all(ctx, login_notification);

    if (!success) {
        LOG_F(ERROR, "send error");
        exit(EXIT_FAILURE);
    }


    // 添加消息, Message 类型存储不了  info ？？？？？？
    ctx.msg_map[pwd]->add_one(response);



}

#endif