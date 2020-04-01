
// 存储全局的消息， 最多存储 n 条
class message {
private:
    // 支持存储的最大消息
    int maxLen;
    // 当前的存储位置
    int index;
    //最晚的消息编号
    int lastId;
    bool save2file;
    vector <im_message::Message> msg;
    vector<int> msg_id;

    message() {
        message(1000);// 默认 1000条
    }

    message(int maxLen) {
        index = 0;
        this->maxLen = maxLen;
        save2file = false;
    }

public:
    void add_one(im_message::Message msg, int id) {//消息的全局编号

        if (msg.size() < maxLen) {
            this->msg.push_back(msg);
            this->msg_id.push_back(id);
        } else {
            if (index >= maxLen) index = 0;
            this->msg[index] = msg;
            this->msg_id[index] = id;
            //! 会不会内存泄漏？  原来的string 对象去哪里了？
            index++;
        }
        if (save2file) save2file();
    }

    vector <pair<im_message::Message, int>> get_all() {
        vector <pair<im_message::Message, int>> ret;
        if (this->msg.size() < maxLen) {
            for (int i = 0; i < msg.size(); i++) ret.push_back(make_pair(msg[i], msg_id[i]));
        } else {
            int in = index + 1;
            while (in < maxLen) ret.push_back(make_pair(msg[in], msg_id[in++]));
            in = 0;
            while (in <= index) ret.push_back(make_pair(msg[in], msg_id[in++]));
        }
        return ret;
    }

    // 获取 id 以后的所有消息
    void get_by_id(int id) {

    }

    bool save2file(string msg) {
        // 保存到文件？？？
    }


};