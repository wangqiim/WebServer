#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include "Poller.h"

Poller::Poller() : pollfd_(-1), eventlist_(EVENTNUM), channelmap_() {
  this->pollfd_ = epoll_create(256);
  assert(this->pollfd_ != -1);
}

Poller::~Poller() { close(this->pollfd_); }

void Poller::poll(ChannelList& activechannellist) {
  int timeout = EPOLLTIMEOUT;
  int nfds =
      epoll_wait(this->pollfd_, &this->eventlist_[0],
                 static_cast<int>(this->eventlist_.size()), timeout);
  assert(nfds != -1);

  for (int i = 0; i < nfds; ++i) {
    int events       = this->eventlist_[i].events;
    Channel* chan = static_cast<Channel*>(eventlist_[i].data.ptr);
    int fd           = chan->GetFd();
    assert(this->channelmap_.count(fd) != 0);
    chan->SetEvents(events);
    activechannellist.push_back(chan);
  }

  if (nfds == static_cast<int>(this->eventlist_.size()))
    eventlist_.resize(nfds * 2);
}

void Poller::AddChannel(Channel* chan) {
  int fd = chan->GetFd();
  struct epoll_event ev;
  ev.events             = chan->GetEvents();
  ev.data.ptr           = chan;
  this->channelmap_[fd] = chan;

  int ret = epoll_ctl(this->pollfd_, EPOLL_CTL_ADD, fd, &ev);
  assert(ret != -1);
}

void Poller::RemoveChannel(Channel* chan) {
  int fd = chan->GetFd();
  struct epoll_event ev;
  ev.events   = chan->GetEvents();
  ev.data.ptr = chan;
  channelmap_.erase(fd);

  int ret = epoll_ctl(pollfd_, EPOLL_CTL_DEL, fd, &ev);
  assert(ret != -1);
}

void Poller::UpdateChannel(Channel* chan) {
  int fd = chan->GetFd();
  struct epoll_event ev;
  ev.events   = chan->GetEvents();
  ev.data.ptr = chan;

  int ret = epoll_ctl(pollfd_, EPOLL_CTL_MOD, fd, &ev);
  assert(ret != -1);
}
