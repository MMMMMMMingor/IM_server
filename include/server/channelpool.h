//
// Created by Firefly on 2020/4/8.
//

#ifndef IM_SERVER_CHANNELPOOL_H
#define IM_SERVER_CHANNELPOOL_H

#include "channel.h"

/**
 * 1、 在dispather给每一个 accept的fd 一个chanel
 * 2、 在 chanel 中存储每一个fd 的buf，用于  处理半包问题和粘包问题
 * 3、 。。
 */


class ChannelPool {
public:
    Channel *get_channel_by_fd(int,int);

    void add_channel_by_fd(int,Channel*);

private:
    std::unordered_map<int, Channel*> m_channel_pool;
};


#endif //IM_SERVER_CHANNELPOOL_H
