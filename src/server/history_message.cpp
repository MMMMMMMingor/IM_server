//
// Created by Mingor on 2020/4/5.
//
#include <message.pb.h>
#include <server/histroy_message.h>

// 默认 1000条
HistoryMessage::HistoryMessage() : HistoryMessage{1000} {}

HistoryMessage::HistoryMessage(int max_len) {
  id = 0;
  this->max_len = max_len;
  save2file = false;
}

void HistoryMessage::add_one(im_message::Message msg) { //消息的全局编号
  this->msg.push_back(msg);
  if (id++ > max_len)
    this->msg.pop_front();
}

int HistoryMessage::get_id() { return this->id; }

std::list<im_message::Message> HistoryMessage::get_all() { return msg; }

// 获取 id 以后的所有消息, 先用  void测试
void HistoryMessage::get_by_id(int id) {std::cout << "old_id: " << id << "cur_id: " << this->id << std::endl;}

bool HistoryMessage::save(std::string msg) {
  // 保存到文件？？？
  return false;
}
