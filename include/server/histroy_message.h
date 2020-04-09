/*
 * @Author: Firefly
 * @Date: 2020-04-01 11:26:22
 * @Descripttion:
 * @LastEditTime: 2020-04-01 23:34:37
 */
#ifndef HISTORY_MESSAGE_H
#define HISTORY_MESSAGE_H

#include "message.pb.h"
#include <list>

// 存储全局的消息， 最多存储 n 条
class HistoryMessage {

public:
  HistoryMessage();

  explicit HistoryMessage(int max_len);

  void add_one(im_message::Message msg);
  int get_id();

  std::list<im_message::Message> get_all();

  // 获取 id 以后的所有消息
  void get_by_id(int id);

  bool save(std::string msg);

private:
  // 支持存储的最大消息
  int max_len;
  // 当前的存储位置
  int room_id{};
  // 这个房间号的id 消息序号
  int id;
  bool save2file{false};
  std::list<im_message::Message> msg;
};

struct user_msg{
    int m_msg_id; // 存储上一次的下线的消息id
};



#endif