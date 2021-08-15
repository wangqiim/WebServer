#include <iostream>
#include <sys/epoll.h>
#include <assert.h>
#include <stdio.h>
#include "Channel.h"

Channel::Channel() {}

Channel::~Channel() {}

void Channel::HandleEvent() {
  switch (this->events_) {
  case EPOLLRDHUP:
    // Stream socket peer closed connection, or shut down writing half
    // of connection
    this->closehandler_();
    break;
  case EPOLLPRI:
    // There is urgent data available for read(2) operations.
  case EPOLLIN: // if read return 0, receive FIN
    this->readhandler_();
    break;
  case EPOLLOUT:
    this->writehandler_();
    break;
  default:
    perror("unexpected happen\n");
    this->errorhandler_();
  }
}
