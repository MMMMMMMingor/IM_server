#ifndef HANDLE_HPP
#define HANDLE_HPP

#include "common.hpp"
#include "loguru.hpp"

/**
 * 广播 handler
 * @param client_fd 客户端socket文件描述符
 * @return 读取的长度
 */
int broadcast_message_handler(int client_fd) {

    char buf[BUF_SIZE];
    char message[BUF_SIZE];
    bzero(buf, BUF_SIZE);//清空初始化
    //bzero(buf, BUF_SIZE);


    LOG_F(INFO, "read from client(clientID = %d)", client_fd);
    int len = 0, tmp = 0;
    bool isClose = true;
    string info;

    while (true) {
        tmp = recv(client_fd, buf, BUF_SIZE, 0);
        if (isClose && tmp == 0) break;
        if (tmp != 0) {
            isClose = false;
            len += tmp;     //总的长度
            for (int i = 0; i < tmp; i++) info += buf[i];
        } else break;
    }


    if (0 == len) {              // 关了， 关掉服务， 删除 user
        close(client_fd);
        clients_list.remove(client_fd);
        LOG_F(INFO,
              "ClientID = %d closed. now there are %d client in the char room",
              client_fd, (int) clients_list.size());
    } else {
        if (1 == clients_list.size()) {
            send(client_fd, CAUTION, strlen(CAUTION), 0);
            return 0;
        }

        sprintf(message, SERVER_MESSAGE, client_fd, buf);

        for (const auto &socket : clients_list) {
            if (socket != client_fd) {
                if (send(socket, message, BUF_SIZE, 0) < 0) {
                    LOG_F(ERROR, "board cast error");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    return len;
}


#endif // !HANDLE_HPP
