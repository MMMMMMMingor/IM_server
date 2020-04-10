//
// Created by Firefly on 2020/4/8.
//

#include <loguru.hpp>
#include <server/channelpool.h>

ChannelPool *ChannelPool::m_instance = nullptr;

ChannelPool *ChannelPool::get_instance() {
  if (ChannelPool::m_instance == nullptr) {
    ChannelPool::m_instance = new ChannelPool;
  }
  return ChannelPool::m_instance;
}

/**
 * 通过fd 给分配一个channel，
 *
 * bug： 如果 同一个时间在很短的时间内到达可能创建了两个
 * @param fd
 * @return
 */
Channel *ChannelPool::get_channel_by_fd(int fd,
                                        int listen_fd) { // TODO 未实现线程安全

  LOG_F(INFO, "fd: %d , listen_fd: %d", fd, listen_fd);

  if (fd == listen_fd) {
    auto *channel = new Channel(fd); // 创建全局的channel
    if (this->m_channel_pool.size() > 1000)
      LOG_F(INFO, "i have create 1000");
    return channel;
  }
  // 可能是同一个事件还没有创建好， 因此要等上一个时间创建好
  while (this->m_channel_pool.find(fd) == m_channel_pool.end())
    LOG_F(WARNING, "warning, warning");

  return this->m_channel_pool[fd];
}

void ChannelPool::add_channel_by_fd(int fd) {
  this->m_channel_pool[fd] = new Channel(fd);
}
