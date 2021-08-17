#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include <functional>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <atomic>
#include <mutex>
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
  /* Close 和Send 只会被服务层call */
  void Close();
  void Send(std::string& msg);

  /* 注册到Channel上的回调，内嵌了上层的回调函数，只会被特定的loop线程call到*/
  void HandleRead();
  void HandleWrite();
  void HandleError();
  void HandleClose();

  /* Channel上的回调再执行完Handlexxx()的逻辑后，会执行上层的回调函数 */
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

  void AddChannelToPoller() {
    this->loop_->AddTask(std::bind(&EventLoop::AddChannelToPoller,
                                   this->loop_, this->channel_));
  }

  struct sockaddr_in* GetClientAddr() {
    return &this->clientaddr_;
  }

private:
  EventLoop* loop_;
  Channel* channel_;
  int fd_;
  struct sockaddr_in clientaddr_;
  std::atomic<bool> halfclose_; /* 由于io线程和worker线程竞争 */
  bool closed_; /* 引入消除sig segment，防止channel触发多次关闭conn */

  std::mutex mutex_;
  std::string tmp_; /* 由于io线程和worker线程竞争 */
  std::string bufferin_;
  std::string bufferout_;

  MessageCallback messagecallback_;
  Callback sendcompletecallback_;
  Callback closecallback_;
  Callback errorcallback_;

  TaskCallback connectioncleanup_;
};

#endif
