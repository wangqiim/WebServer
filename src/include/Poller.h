#ifndef _POLLER_H_
#define _POLLER_H_

#include <vector>
#include <map>
#include <sys/epoll.h>
#include "Channel.h"

#define EVENTNUM 4096
#define EPOLLTIMEOUT 1000

class Poller {
public:
  typedef std::vector<Channel*> ChannelList;

  Poller();
  ~Poller();

  void poll(ChannelList& activechannellist);
  void AddChannel(Channel* chan);
  void RemoveChannel(Channel* chan);
  void UpdateChannel(Channel* chan);

private:
  int pollfd_;
  std::vector<struct epoll_event> eventlist_;
  std::map<int, Channel*> channelmap_;
};

#endif