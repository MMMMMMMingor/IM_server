/*
 * @Author: Firefly
 * @Date: 2020-04-01 11:26:22
 * @Descripttion: 
 * @LastEditTime: 2020-04-01 23:34:37
 */

#include "common.hpp"
#include "loguru.hpp"
#include <loguru.hpp>


// 存储全局的消息， 最多存储 n 条
class history_message {
private:
    // 支持存储的最大消息
    int max_len;
    // 当前的存储位置
    int room_id;
    // 这个房间号的id 消息序号
    int id;
    bool save2file;
    std::list<im_message::Message> msg;

public:
    history_message() {
        history_message(1000);// 默认 1000条
    }

    history_message(int max_len) {
        id = 0;
        this->max_len = max_len;
        save2file = false;
        id = 0;
    }


    void add_one(im_message::Message msg) {//消息的全局编号
        this->msg.push_back(msg);
        if(id++ > max_len) this->msg.pop_front();
    }
    int get_id(){
        return  this->id;
    }

    std::vector<im_message::Message> get_all() {

    }

    // 获取 id 以后的所有消息
    void get_by_id(int id) {
       
    }

    bool save(std::string msg) {
        // 保存到文件？？？
        return false;
    }


};
















