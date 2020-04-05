//
// Created by Mingor on 2020/4/1.
//
#include "server/keepalive.h"
#include <chrono>
#include <iostream>
#include <ratio>
#include <thread>

using namespace std;

int main() {

  using TIME = decltype(chrono::system_clock::now());
  TIME start = chrono::system_clock::now();
  //  auto start = chrono::high_resolution_clock::now();
  this_thread::sleep_for(chrono::seconds(1));
  TIME end = chrono::system_clock::now();
  //  auto end = chrono::high_resolution_clock::now();
  chrono::duration<double> diff = end - start;
  auto nanos = chrono::duration_cast<chrono::nanoseconds>(end - start);
  chrono::duration<double, nano> fp_nanos = end - start;
  cout << "use " << diff.count() << " s\n";
  cout << "use " << nanos.count() << " nanos\n";
  cout << "use " << fp_nanos.count() << " nanos\n";
  return 0;
}