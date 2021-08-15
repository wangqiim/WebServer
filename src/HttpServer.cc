#include <iostream>
#include <functional>
#include "HttpServer.h"

HttpServer::HttpServer(EventLoop* loop, int port)
    : tcpserver_(loop, port) {
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
}

HttpServer::~HttpServer() {}

void HttpServer::HandleNewConnection(TcpConnection* tcpConn) {
  HttpSession* httpSession         = new HttpSession();
  this->httpsessionnlist_[tcpConn] = httpSession;

  // char remote[INET_ADDRSTRLEN];
  // printf("new conn with ip: %s and port: %d\n",
  //        inet_ntop(AF_INET, &tcpConn->GetClientAddr()->sin_addr, remote,
  //                  INET_ADDRSTRLEN),
  //        ntohs(tcpConn->GetClientAddr()->sin_port));
}

void HttpServer::HandleMessage(TcpConnection* tcpConn, std::string& s) {
  HttpSession* session = this->httpsessionnlist_[tcpConn];
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
  HttpSession* session = httpsessionnlist_[tcpConn];
  httpsessionnlist_.erase(tcpConn);
  delete session;
}

void HttpServer::HandleError(TcpConnection* tcpConn) {
  HttpSession* session = httpsessionnlist_[tcpConn];
  httpsessionnlist_.erase(tcpConn);
  delete session;
}

void HttpServer::Start() { this->tcpserver_.Start(); }
