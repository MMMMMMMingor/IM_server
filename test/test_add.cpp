#include <iostream>
#include <server/ThreadPool/ThreadPool.h>
#include <unistd.h>



void work(int a, int b) {
    std::cout << "this is work" << a << " " << b << std::endl;
    sleep(1);
}


int main(int argc, char *argv[]) {
//  int a = atoi(argv[1]);
//  int b = atoi(argv[2]);
//
//  std::cout << a + b << std::endl;

    ThreadPool p(static_cast<size_t>(4), static_cast<size_t >(100));
    for (int i = 0; i < 100; i++) p.addTask(work,ARG(i,2));
    pause();
    return 0;
}