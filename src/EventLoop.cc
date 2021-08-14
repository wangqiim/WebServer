#include <iostream>
#include "EventLoop.h"

EventLoop::EventLoop()
    : functorlist_(), channellist_(), activechannellist_(), poller_(),
      stop_(true) {}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
  this->stop_ = false;
  while (!this->stop_) {
    poller_.poll(this->activechannellist_);
    for (Channel* channel : this->activechannellist_)
      channel->HandleEvent();
    this->activechannellist_.clear();
    this->ExecuteTask();
  }
}