#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include <functional>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Channel.h"
#include "EventLoop.h"

class TcpConnection {
public:
  typedef std::function<void(TcpConnection*)> Callback;
  typedef std::function<void(TcpConnection*, std::string&)>
      MessageCallback;
  typedef std::function<void()> TaskCallback;

  TcpConnection(EventLoop* loop, int fd, struct sockaddr_in clientaddr);
  ~TcpConnection();

  int Fd() { return this->fd_; }
  void Send(std::string& msg);

  void HandleRead();
  void HandleWrite();
  void HandleError();
  void HandleClose();

  void SetMessaeCallback(MessageCallback cb) {
    this->messagecallback_ = cb;
  }

  void SetSendCompleteCallback(Callback cb) {
    this->sendcompletecallback_ = cb;
  }

  void SetCloseCallback(Callback cb) { this->closecallback_ = cb; }

  void SetErrorCallback(Callback cb) { this->errorcallback_ = cb; }

  void SetConnectionCleanUp(TaskCallback cb) {
    this->connectioncleanup_ = cb;
  }

  struct sockaddr_in* GetClientAddr() {
    return &this->clientaddr_;
  }

private:
  EventLoop* loop_;
  Channel* channel_;
  int fd_;
  struct sockaddr_in clientaddr_;
  bool halfclose_;

  std::string bufferin_;
  std::string bufferout_;

  MessageCallback messagecallback_;
  Callback sendcompletecallback_;
  Callback closecallback_;
  Callback errorcallback_;

  TaskCallback connectioncleanup_;
};

#endif
