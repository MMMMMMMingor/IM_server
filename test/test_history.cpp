//
// Created by Firefly on 2020/4/1.
//

#ifndef IM_SERVER_TEST_HISTORY_CPP_H
#define IM_SERVER_TEST_HISTORY_CPP_H


#include <message.pb.h>
#include <common.hpp>


int main() {


    for (int i = 0; i < 12; i++) {
        im_message::Message messag1e;
        history_msg->add_one(messag1e);
        std::cout << history_msg->get_id() << " ";

        std::cout << history_msg->get_all().size() << std::endl;

    }


    return 0;
}


#endif //IM_SERVER_TEST_HISTORY_CPP_H
