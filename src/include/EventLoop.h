#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include "Channel.h"
#include "Poller.h"

class EventLoop {
public:
  typedef std::function<void()> Functor;
  typedef std::vector<Channel*> ChannelList;

  EventLoop();
  ~EventLoop();

  void loop();

  void AddChannelToPoller(Channel* chan) {
    this->poller_.AddChannel(chan);
  }

  void RemoveChannelToPoller(Channel* chan) {
    this->poller_.RemoveChannel(chan);
  }

  void UpdateChannelToPoller(Channel* chan) {
    this->poller_.UpdateChannel(chan);
  }

  void AddTask(Functor func) {
    {
      std::lock_guard<std::mutex> lock(this->mutex_);
      this->functorlist_.push_back(func);
    }
    this->WakeUp();
  }

  void ExecuteTask() {
    std::vector<Functor> functorlist;
    {
      std::lock_guard<std::mutex> lock(this->mutex_);
      functorlist.swap(this->functorlist_);
    }
    for (Functor& func : functorlist)
      func();
  }

  void Stop() { this->stop_ = true; }

  std::thread::id GetTID() { return this->tid_; }
  void WakeUp();
  void HandleRead();
  void HandleError();

private:
  std::vector<Functor> functorlist_;
  ChannelList channellist_;
  ChannelList activechannellist_;
  Poller poller_;
  bool stop_;

  std::thread::id tid_;
  std::mutex mutex_;
  int wakeupfd_;
  Channel wakeupChannel_;
};

#endif