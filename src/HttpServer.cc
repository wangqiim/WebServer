#include <iostream>
#include <functional>
#include "HttpServer.h"

HttpServer::HttpServer(EventLoop* loop, int port, int ioThreadNum,
                       int workerThreadNum)
    : tcpserver_(loop, port, ioThreadNum),
      threadPool_(workerThreadNum) {
  tcpserver_.SetNewConnCallback(std::bind(
      &HttpServer::HandleNewConnection, this, std::placeholders::_1));
  tcpserver_.SetMessageCallback(std::bind(&HttpServer::HandleMessage,
                                          this, std::placeholders::_1,
                                          std::placeholders::_2));
  tcpserver_.SetSendCompleteCallback(std::bind(
      &HttpServer::HandleSendComplete, this, std::placeholders::_1));
  tcpserver_.SetCloseCallback(
      std::bind(&HttpServer::HandleClose, this, std::placeholders::_1));
  tcpserver_.SetErrorCallback(
      std::bind(&HttpServer::HandleError, this, std::placeholders::_1));

  this->threadPool_.Start();
}

HttpServer::~HttpServer() {}

void HttpServer::HandleNewConnection(TcpConnection* tcpConn) {
  HttpSession* httpSession = new HttpSession();
  {
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->httpsessionnlist_[tcpConn] = httpSession;
  }

  // char remote[INET_ADDRSTRLEN];
  // printf("new conn with ip: %s and port: %d\n",
  //        inet_ntop(AF_INET, &tcpConn->GetClientAddr()->sin_addr,
  //        remote,
  //                  INET_ADDRSTRLEN),
  //        ntohs(tcpConn->GetClientAddr()->sin_port));
}

void HttpServer::HandleMessage(TcpConnection* tcpConn, std::string& s) {
  HttpSession* session = NULL;
  {
    std::lock_guard<std::mutex> lock(this->mutex_);
    session = this->httpsessionnlist_[tcpConn];
  }
  if (this->threadPool_.GetThreadNum() > 0) {
    this->threadPool_.AddTask([=, &s]() {
      if (s.empty())
        return;
      session->PraseHttpRequest(s);
      session->HttpProcess();
      std::string msg;
      session->AddToBuf(msg);
      tcpConn->Send(msg);
      if (!session->KeepAlive()) {
        // 短链接
        tcpConn->HandleClose();
      }
    });
    return;
  }
  session->PraseHttpRequest(s);
  session->HttpProcess();
  std::string msg;
  session->AddToBuf(msg);
  tcpConn->Send(msg);
  if (!session->KeepAlive()) {
    // 短链接
    tcpConn->HandleClose();
  }
}

void HttpServer::HandleSendComplete(TcpConnection* tcpConn) {}

void HttpServer::HandleClose(TcpConnection* tcpConn) {
  std::lock_guard<std::mutex> lock(this->mutex_);
  HttpSession* session = httpsessionnlist_[tcpConn];
  httpsessionnlist_.erase(tcpConn);
  delete session;
}

void HttpServer::HandleError(TcpConnection* tcpConn) {
  std::lock_guard<std::mutex> lock(this->mutex_);
  HttpSession* session = httpsessionnlist_[tcpConn];
  httpsessionnlist_.erase(tcpConn);
  delete session;
}

void HttpServer::Start() { this->tcpserver_.Start(); }
