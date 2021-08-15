#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <string>
#include <map>
#include <mutex>
#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "HttpSession.h"

class HttpServer {
private:
  void HandleNewConnection(TcpConnection* ptcpconn);
  void HandleMessage(TcpConnection* ptcpconn, std::string& s);
  void HandleSendComplete(TcpConnection* ptcpconn);
  void HandleClose(TcpConnection* ptcpconn);
  void HandleError(TcpConnection* ptcpconn);

  std::mutex mutex_;
  TcpServer tcpserver_;
  std::map<TcpConnection*, HttpSession*> httpsessionnlist_;

public:
  HttpServer(EventLoop* loop, int port, int threadNum);
  ~HttpServer();

  void Start();
};

#endif
