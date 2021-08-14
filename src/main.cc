#include <iostream>

#include "EventLoop.h"
#include "EchoServer.h"

int main() {
  std::cout << "server is starting on port: 8080" << std::endl;
  EventLoop loop;
  EchoServer echoServer(&loop, 8088);
  echoServer.Start();
  loop.loop();
  std::cout << "server is running on port: 8080" << std::endl;
  return 0;
}