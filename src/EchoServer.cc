#include <iostream>
#include <stdio.h>
#include <functional>
#include "EchoServer.h"

EchoServer::EchoServer(EventLoop* loop, int port, int threadNum)
    : tcpserver_(loop, port, threadNum) {
  tcpserver_.SetNewConnCallback(std::bind(
      &EchoServer::HandleNewConnection, this, std::placeholders::_1));
  tcpserver_.SetMessageCallback(std::bind(&EchoServer::HandleMessage,
                                          this, std::placeholders::_1,
                                          std::placeholders::_2));
  tcpserver_.SetSendCompleteCallback(std::bind(
      &EchoServer::HandleSendComplete, this, std::placeholders::_1));
  tcpserver_.SetCloseCallback(
      std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
  tcpserver_.SetErrorCallback(
      std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
}

EchoServer::~EchoServer() {}

void EchoServer::Start() { tcpserver_.Start(); }

void EchoServer::HandleNewConnection(TcpConnection* tcpConn) {
  char remote[INET_ADDRSTRLEN];
  printf("new conn with ip: %s and port: %d\n",
         inet_ntop(AF_INET, &tcpConn->GetClientAddr()->sin_addr, remote,
                   INET_ADDRSTRLEN),
         ntohs(tcpConn->GetClientAddr()->sin_port));
}

void EchoServer::HandleMessage(TcpConnection* tcpConnection,
                               std::string& s) {
  std::string msg;
  msg.swap(s);
  msg.insert(0, "[reply msg] ");
  tcpConnection->Send(msg);
}

void EchoServer::HandleSendComplete(TcpConnection* tcpConnection) {
  // std::cout << "Message send complete" << std::endl;
}

void EchoServer::HandleClose(TcpConnection* tcpConn) {
  char remote[INET_ADDRSTRLEN];
  printf("close conn with ip: %s and port: %d\n",
         inet_ntop(AF_INET, &tcpConn->GetClientAddr()->sin_addr, remote,
                   INET_ADDRSTRLEN),
         ntohs(tcpConn->GetClientAddr()->sin_port));
}

void EchoServer::HandleError(TcpConnection* tcpConnection) {
  std::cout << "EchoServer a conn error" << std::endl;
}
