#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TcpServer.h"

void Setnonblocking(int fd);

TcpServer::TcpServer(EventLoop* loop, int port, int threadNum)
    : serversocket_(), loop_(loop), serverchannel_(),
      eventLoopThreadPool_(loop_, threadNum), conncount_(0) {
  this->serversocket_.SetReuseAddr();
  this->serversocket_.BindAddress(port);
  this->serversocket_.Listen();
  this->serversocket_.Setnonblocking();

  this->serverchannel_.SetFd(serversocket_.Fd());
  this->serverchannel_.SetReadHandle(
      std::bind(&TcpServer::OnNewConnection, this));
  this->serverchannel_.SetErrorHandle(
      std::bind(&TcpServer::OnConnectionError, this));
}

TcpServer::~TcpServer() {}

void TcpServer::Start() {
  this->eventLoopThreadPool_.Start();
  this->serverchannel_.SetEvents(EPOLLIN | EPOLLET);
  this->loop_->AddChannelToPoller(&this->serverchannel_);
}

void TcpServer::OnNewConnection() {
  struct sockaddr_in clientaddr;
  int clientfd;
  // nonblocking and epoll edge trigger
  while ((clientfd = serversocket_.Accept(clientaddr)) > 0) {
    if (++this->conncount_ >= MAXCONNECTION) {
      close(clientfd);
      this->conncount_--;
      continue;
    }
    Setnonblocking(clientfd);
    EventLoop* loopWorker = this->eventLoopThreadPool_.GetNextLoop();
    TcpConnection* tcpConnection =
        new TcpConnection(loopWorker, clientfd, clientaddr);
    tcpConnection->SetMessaeCallback(this->messagecallback_);
    tcpConnection->SetSendCompleteCallback(this->sendcompletecallback_);
    tcpConnection->SetCloseCallback(this->closecallback_);
    tcpConnection->SetErrorCallback(this->errorcallback_);
    tcpConnection->SetConnectionCleanUp(
        std::bind(&TcpServer::RemoveConnection, this, tcpConnection));
    {
      std::lock_guard<std::mutex> lock(this->mutex_);
      this->tcpconnlist_[clientfd] = tcpConnection;
    }

    this->newconnectioncallback_(tcpConnection);

    tcpConnection->AddChannelToPoller();
  }
}

void TcpServer::RemoveConnection(TcpConnection* tcpConnection) {
  std::lock_guard<std::mutex> lock(this->mutex_);
  --this->conncount_;
  this->tcpconnlist_.erase(tcpConnection->Fd());
  delete tcpConnection;
}

void TcpServer::OnConnectionError() {
  std::cout << "UNKNOWN EVENT" << std::endl;
  this->serversocket_.Close();
}

void Setnonblocking(int fd) {
  int opts = fcntl(fd, F_GETFL);
  assert(opts >= 0);
  int ret = fcntl(fd, F_SETFL, opts | O_NONBLOCK);
  assert(ret >= 0);
}
