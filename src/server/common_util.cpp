//
// Created by Mingor on 2020/4/5.
//
#include <common.hpp>
#include <server/common_util.hpp>

/**
 * 获取当前时间戳
 * @return
 */
std::string getTime() {
    time_t time_stamp;
    time(&time_stamp);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&time_stamp));
    return tmp; //自动转型
}

/**
 *设置非阻塞
 */
int setNonblock(int sockfd) {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

/**
 * 将文件描述符 fd 添加到 epollfd 标示的内核事件表中,
 * 并注册 EPOLLIN 和 EPOOLET 事件,
 * EPOLLIN 是数据可读事件；EPOOLET 表明是 ET 工作方式。
 * 最后将文件描述符设置非阻塞方式
 * @param epoll_fd :epoll 文件描述符
 * @param fd      :文件描述符
 * @param enable_et:enable_et = true,
 * 是否采用epoll的ET(边缘触发)工作方式；否则采用LT(水平触发)工作方式
 */
void add_fd(int epollfd, int fd, bool enable_et) {
    struct epoll_event ev{};
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et) {
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setNonblock(fd);
}

/**
 * 发送数据（线程安全）
 * @param ctx
 * @param out_message
 */
void send_message_safe(Session *session,
                       const im_message::Message &out_message) {


    using namespace google::protobuf::io;
    LOG_F(INFO, "--------------send_message_safe-------------------");

    std::lock_guard<std::mutex> lg(session->get_mutex());
    int fd = session->get_socket_fd();

    ZeroCopyOutputStream *raw_output = new FileOutputStream(fd);

    auto coded_output = new CodedOutputStream(raw_output);

    uint32_t len = out_message.ByteSizeLong();
    char *buf = new char[len];
    out_message.SerializeToArray(buf, len);

    coded_output->WriteVarint32(len);
    coded_output->WriteRaw(buf, len);

    delete[] buf;
    delete coded_output;
    delete raw_output;
}

/**
 * 发送数据（线程安全）
 * @param ctx
 * @param out_message
 */
void send_message(Context &ctx, const im_message::Message &out_message) {
    LOG_F(INFO, "--------------send_message-------------------");
    Session *session = ctx.session_pool.find_session(out_message.session_id());

    send_message_safe(session, out_message);
}

/**
 * 广播消息
 * @param Context       上下文
 * @param message
 */
void board_cast_message(Context &ctx, const im_message::Message &message) {
    LOG_F(INFO, "--------------board_cast_message-------------------");

    auto lambda = [&message](uint64_t session_id, Session *session) {
        send_message_safe(session, message);
    };

    ctx.session_pool.for_each(lambda);
}

void board_cast_message_by_room_id(Context &ctx,
                                   const im_message::Message &message,
                                   const std::string &room_id) {

    LOG_F(INFO, "--------------board_cast_message_by_room_id-------------------");

    auto lambda = [&ctx, &message, &room_id](uint64_t session_id, Session *session) {
        if (room_id != session->get_room_name())
            return; // 相同的 room id 才执行发送

        ctx.msg_map[room_id]->add_one(message);   //只存储播报的消息

        send_message_safe(session, message);
    };

    ctx.session_pool.for_each(lambda);
}

/**
 * 通知某一个房间的所有用户
 * @param ctx
 * @param message
 */
void notification_by_room_id(Context &ctx, const std::string &message,
                             const std::string &room_id) {
    LOG_F(INFO, "--------------notification_by_room_id-------------------");

    im_message::Message response;
    response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
    auto *message_notification = new im_message::MessageNotification;

    message_notification->set_json(message);
    message_notification->set_timestamp(getTime());
    response.set_allocated_notification(message_notification);

    board_cast_message_by_room_id(ctx, response, room_id);
}

/**
 * 通知所有用户
 * @param Context       上下文
 * @param message       消息
 */
void notification_all(Context &ctx, const std::string &message) {
    LOG_F(INFO, "--------------notification_all-------------------");

    im_message::Message response;
    response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
    auto *message_notification = new im_message::MessageNotification;
    //! 在哪里 del？？？？？？？？？？？？？？？？？？？
    message_notification->set_json(message);
    message_notification->set_timestamp(getTime());
    response.set_allocated_notification(message_notification);

    board_cast_message(ctx, response);
}

/**
 * 消息通知单个用户
 * @param Context       上下文
 * @param message       消息
 */
void notification_one(Context &ctx, uint64_t session_id,
                      const std::string &message) {
    LOG_F(INFO, "--------------notification_one-------------------");

    im_message::Message response;
    response.set_session_id(session_id);
    response.set_type(im_message::HeadType::MESSAGE_NOTIFICATION);
    auto *message_notification = new im_message::MessageNotification;
    message_notification->set_json(message);
    response.set_allocated_notification(message_notification);

    send_message(ctx, response);
}

/**
 * 检测事件类型
 * @param type 事件类型
 */
void check_event_type(uint32_t type) {
    if (type & EPOLLIN)
        LOG_F(INFO, "type check:\tEPOLLIN");
    if (type & EPOLLOUT)
        LOG_F(INFO, "type check:\tEPOLLOUT");
    if (type & EPOLLRDHUP)
        LOG_F(INFO, "type check:\tEPOLLRDHUP ");
    if (type & EPOLLPRI)
        LOG_F(INFO, "type check:\tEPOLLPRI");
    if (type & EPOLLERR)
        LOG_F(INFO, "type check:\tEPOLLERR");
    if (type & EPOLLHUP)
        LOG_F(INFO, "type check:\tEPOLLHUP");
    if (type & EPOLLET)
        LOG_F(INFO, "type check:\tEPOLLET");
    if (type & EPOLLONESHOT)
        LOG_F(INFO, "type check:\tEPOLLONESHOT ");
    if (type & EPOLLWAKEUP)
        LOG_F(INFO, "type check:\tEPOLLWAKEUP ");
}
