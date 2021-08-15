#ifndef _EVENTLOOP_THREAD_POOL_H_
#define _EVENTLOOP_THREAD_POOL_H_

#include <iostream>
#include <vector>
#include <string>
#include "EventLoop.h"
#include "EventLoopThread.h"

class EventLoopThreadPool {
private:
  EventLoop* mainloop_;
  int threadnum_;
  std::vector<EventLoopThread*> threadlist_;
  int index_;

public:
  EventLoopThreadPool(EventLoop* mainloop, int threadnum = 0);
  ~EventLoopThreadPool();

  void Start();

  // RoundRobin
  EventLoop* GetNextLoop();
};

#endif