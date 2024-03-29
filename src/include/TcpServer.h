#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <atomic>
#include <functional>
#include <string>
#include <map>
#include <mutex>
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

#define MAXCONNECTION 10000

class TcpServer {
public:
  typedef std::function<void(TcpConnection*, std::string&)>
      MessageCallback;
  typedef std::function<void(TcpConnection*)> Callback;
  TcpServer(EventLoop* loop, int port, int threadNum = 0);
  ~TcpServer();

  void Start();

  void SetNewConnCallback(Callback cb) {
    this->newconnectioncallback_ = cb;
  }

  void SetMessageCallback(MessageCallback cb) {
    this->messagecallback_ = cb;
  }

  void SetSendCompleteCallback(Callback cb) {
    this->sendcompletecallback_ = cb;
  }

  void SetCloseCallback(Callback cb) { this->closecallback_ = cb; }

  void SetErrorCallback(Callback cb) { this->errorcallback_ = cb; }

private:
  Socket serversocket_;
  EventLoop* loop_;
  Channel serverchannel_;
  EventLoopThreadPool eventLoopThreadPool_;
  std::map<int, TcpConnection*> tcpconnlist_;

  std::atomic<int> conncount_;
  std::mutex mutex_;

  Callback newconnectioncallback_;
  MessageCallback messagecallback_;
  Callback sendcompletecallback_;
  Callback closecallback_;
  Callback errorcallback_;

  void OnNewConnection();
  void RemoveConnection(TcpConnection* tcpConnection);
  void OnConnectionError();
};

#endif