//
// Created by Firefly on 2020/4/8.
//

#ifndef CHANNEL_POOL_H
#define CHANNEL_POOL_H

#include "server/channel.h"

/**
 * 1、 在dispather给每一个 accept的fd 一个chanel
 * 2、 在 chanel 中存储每一个fd 的buf，用于
 * 处理半包问题和粘包问题 3、 。。
 */

class ChannelPool {
public:
    Channel *get_channel_by_fd(int,int);

    void add_channel_by_fd(int,Channel*);

private:
    std::unordered_map<int, Channel*> m_channel_pool;
};

#endif // CHANNEL_POOL_H
