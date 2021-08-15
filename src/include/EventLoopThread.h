#ifndef _EVENTLOOP_THREAD_H_
#define _EVENTLOOP_THREAD_H_

#include <iostream>
#include <string>
#include <thread>
#include "EventLoop.h"

class EventLoopThread {
private:
  std::thread thread_;
  std::thread::id threadID_;
  std::string threadName_;
  EventLoop* loop_;

public:
  EventLoopThread();
  ~EventLoopThread();

  EventLoop* GetLoop();
  void Start();
  void ThreadFunc();
};

#endif