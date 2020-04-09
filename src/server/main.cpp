//
// Created by Mingor on 2020/4/9.
//

#include <loguru.hpp>
#include <server/server.h>

int main(int argc, char *argv[]) {

  // 初始化 loguru 日志库 // 属于全局的？
  loguru::init(argc, argv);

  Server server;
  server.start();

  return 0;
}