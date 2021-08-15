#include <assert.h>
#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* mainloop,
                                         int threadnum)
    : mainloop_(mainloop), threadnum_(threadnum), threadlist_(),
      index_(0) {
  for (int i = 0; i < this->threadnum_; ++i) {
    EventLoopThread* eventLoopThread = new EventLoopThread();
    this->threadlist_.push_back(eventLoopThread);
  }
}

EventLoopThreadPool::~EventLoopThreadPool() {
  for (int i = 0; i < this->threadnum_; i++)
    delete this->threadlist_[i];
  this->threadlist_.clear();
}

void EventLoopThreadPool::Start() {
  if (this->threadnum_ > 0) {
    for (int i = 0; i < this->threadnum_; ++i)
      this->threadlist_[i]->Start();
  } else {
    std::cout << "eventLoop thread pool is empty!" << std::endl;
  }
}

EventLoop* EventLoopThreadPool::GetNextLoop() {
  if (threadnum_ > 0) {
    EventLoop* loop = this->threadlist_[this->index_]->GetLoop();
    this->index_    = (this->index_ + 1) % this->threadnum_;
    return loop;
  } else {
    return this->mainloop_;
  }
}