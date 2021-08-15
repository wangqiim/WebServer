#ifndef _ECHO_SERVER_H_
#define _ECHO_SERVER_H_

#include <string>
#include "TcpServer.h"
#include "EventLoop.h"
#include "TcpConnection.h"

class EchoServer {
private:
  void HandleNewConnection(TcpConnection* tcpConnection);
  void HandleMessage(TcpConnection* tcpConnection, std::string& s);
  void HandleSendComplete(TcpConnection* tcpConnection);
  void HandleClose(TcpConnection* tcpConnection);
  void HandleError(TcpConnection* tcpConnection);

  TcpServer tcpserver_;

public:
  EchoServer(EventLoop* loop, int port, int threadNum);
  ~EchoServer();

  void Start();
};

#endif