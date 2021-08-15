#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include <functional>
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

  void AddTask(Functor func) { this->functorlist_.push_back(func); }

  void ExecuteTask() {
    for (Functor& func : this->functorlist_)
      func();
    this->functorlist_.clear();
  }

  void Stop() { this->stop_ = true; }

private:
  std::vector<Functor> functorlist_;
  ChannelList channellist_;
  ChannelList activechannellist_;
  Poller poller_;
  bool stop_;
};

#endif