#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>

class Channel {
public:
  typedef std::function<void()> Callback;

  Channel();
  ~Channel();

  void SetFd(int fd) { this->fd_ = fd; }
  int GetFd() { return this->fd_; }

  void SetEvents(uint32_t events) { this->events_ = events; }
  uint32_t GetEvents() { return this->events_; }

  void HandleEvent();
  void SetReadHandle(Callback cb) { this->readhandler_ = cb; }
  void SetWriteHandle(Callback cb) { this->writehandler_ = cb; }
  void SetErrorHandle(Callback cb) { this->errorhandler_ = cb; }
  void SetCloseHandle(Callback cb) { this->closehandler_ = cb; }

private:
  int fd_;
  uint32_t events_;

  Callback readhandler_;
  Callback writehandler_;
  Callback errorhandler_;
  Callback closehandler_;
};

#endif