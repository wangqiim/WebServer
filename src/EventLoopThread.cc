#include <iostream>
#include <sstream>
#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
    : thread_(), threadID_(-1), threadName_("IO thread-"), loop_(NULL) {
}

EventLoopThread::~EventLoopThread() {
  this->loop_->Stop();
  this->thread_.join();
}

EventLoop* EventLoopThread::GetLoop() { return this->loop_; }

void EventLoopThread::Start() {
  this->thread_ = std::thread(&EventLoopThread::ThreadFunc, this);
}

void EventLoopThread::ThreadFunc() {
  EventLoop loop;
  this->loop_ = &loop;

  this->threadID_ = std::this_thread::get_id();
  loop.SetTID(this->threadID_);
  std::stringstream sin;
  sin << this->threadID_;
  this->threadName_ += sin.str();

  std::cout << "IO thread is starting : " << this->threadName_
            << std::endl;
  this->loop_->loop();
}