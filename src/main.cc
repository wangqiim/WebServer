#include <iostream>

#include "EventLoop.h"
#include "EchoServer.h"
#include "HttpServer.h"

int main() {
  EventLoop loop;
#ifdef HTTPSERVER
  std::cout << "http server is starting on port: 8088" << std::endl;
  HttpServer httpServer(&loop, 8088, 2);
  httpServer.Start();
  loop.loop();
#elif ECHOSERVER
  std::cout << "echo server is starting on port: 8088" << std::endl;
  EchoServer echoServer(&loop, 8088, 2);
  echoServer.Start();
  loop.loop();
#else
  return 1;
#endif
  std::cout << "server is running on port: 8088" << std::endl;
  return 0;
}