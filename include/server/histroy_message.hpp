
#include "common.hpp"
#include "loguru.hpp"
#include <loguru.hpp>

// 存储全局的消息， 最多存储 n 条
class history_message {
private:
    // 支持存储的最大消息
    int maxLen;
    // 当前的存储位置
    int index;
    // 全局的 消息id
    int id;
    bool save2file;
    std::vector<im_message::Message> msg;

public:
    history_message() {
        history_message(1000);// 默认 1000条
    }

    history_message(int maxLen) {
        index = 0;
        this->maxLen = maxLen;
        save2file = false;
        id = 0;
    }


    void add_one(im_message::Message msg) {//消息的全局编号

        if (this->msg.size() < maxLen) {
            this->msg.push_back(msg);
        } else {
            if (index >= maxLen) index = 0;
            this->msg[index] = msg;
            //! 会不会内存泄漏？  原来的string 对象去哪里了？
            index++;
        }
        this->id++;
        if (save2file) save("lalala");
//        LOG_F(INFO,"test history_message");
    }
    int get_id(){
        return  this->id;
    }

    std::vector<im_message::Message> get_all() {
        std::vector<im_message::Message> ret;
        if (this->msg.size() < maxLen) {
            for (const auto & i : msg) ret.push_back(i);
        } else {
            int in = index + 1;
            while (in < maxLen) ret.push_back(msg[in++]);
            in = 0;
            while (in <= index) ret.push_back(msg[in++]);
        }
        return ret;
    }

    // 获取 id 以后的所有消息
    void get_by_id(int id) {

    }

    bool save(std::string msg) {
        // 保存到文件？？？
        return false;
    }


};