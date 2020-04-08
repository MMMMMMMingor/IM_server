//
// Created by Mingor on 2020/4/5.
//

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "histroy_message.h"
#include "session.h"
#include "channelpool.h"
#include <sys/epoll.h>
#include <unordered_map>

using msgs_map = std::unordered_map<std::string, HistoryMessage *>;

class Channel;

// 上下文
struct Context {
    epoll_event event;
    int listen_fd;
    int epoll_fd;
    SessionPool &session_pool;
    msgs_map &msg_map;
    Channel *channel;
    bool need_recv_message; // 是否完成数据读取
    std::unordered_map<std::string, user_msg *> &users_info; // 保存离线的消息
    ChannelPool &channel_pool;
};

#endif // CONTEXT_HPP
