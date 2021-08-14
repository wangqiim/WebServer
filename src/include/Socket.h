#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Socket {
private:
  int socketfd_;

public:
  Socket();
  ~Socket();

  int Fd() { return this->socketfd_; }
  void SetReuseAddr();
  void Setnonblocking();
  bool BindAddress(int serverport);
  bool Listen();
  int Accept(struct sockaddr_in& clientaddr);
  bool Close();
};

#endif