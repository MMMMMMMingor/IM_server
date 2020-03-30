//
// Created by Mingor on 2020/3/30.
//

#include <iostream>
#include <server/snowflake.hpp>

int main(int argc, char *argv[]) {
  IdWorker id_worker{1, 1};

  int count = 1000;
  while (count--) {
    std::cout << id_worker.get_next_id() << std::endl;
  }
}