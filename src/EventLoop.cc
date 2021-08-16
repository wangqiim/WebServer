#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <sys/eventfd.h>
#include "EventLoop.h"

int CreateEventFd() {
  int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  assert(evtfd >= 0);
  return evtfd;
}

EventLoop::EventLoop()
    : functorlist_(), channellist_(), activechannellist_(), poller_(),
      stop_(true), tid_(), mutex_(), wakeupfd_(CreateEventFd()),
      wakeupChannel_() {
  this->wakeupChannel_.SetFd(this->wakeupfd_);
  this->wakeupChannel_.SetEvents(EPOLLIN | EPOLLET);
  this->wakeupChannel_.SetReadHandle(
      std::bind(&EventLoop::HandleRead, this));
  this->wakeupChannel_.SetErrorHandle(
      std::bind(&EventLoop::HandleError, this));
  this->AddChannelToPoller(&this->wakeupChannel_);
}

EventLoop::~EventLoop() { close(this->wakeupfd_); }

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

void EventLoop::WakeUp() {
  uint64_t one = 1;
  ssize_t n    = write(this->wakeupfd_, &one, sizeof one);
  assert(n == sizeof one);
}

void EventLoop::HandleRead() {
  uint64_t one = 1;
  ssize_t n    = read(this->wakeupfd_, &one, 8);
  assert(n == sizeof one);
}

void EventLoop::HandleError() { ; }
