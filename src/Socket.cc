#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>
#include "Socket.h"

Socket::Socket() {
  this->socketfd_ = socket(AF_INET, SOCK_STREAM, 0);
  assert(this->socketfd_ != -1);
}

Socket::~Socket() { close(this->socketfd_); }

void Socket::SetReuseAddr() {
  int on  = 1;
  int ret = setsockopt(this->socketfd_, SOL_SOCKET, SO_REUSEADDR, &on,
                       sizeof(on));
  assert(ret != -1);
}

void Socket::Setnonblocking() {
  int opts = fcntl(this->socketfd_, F_GETFL);
  assert(opts >= 0);
  int ret = fcntl(this->socketfd_, F_SETFL, opts | O_NONBLOCK);
  assert(ret >= 0);
}

bool Socket::BindAddress(int serverport) {
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family      = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port        = htons(serverport);
  int ret = bind(this->socketfd_, (struct sockaddr*)&serveraddr,
                 sizeof(serveraddr));
  assert(ret != -1);
  return true;
}

bool Socket::Listen() {
  int ret = listen(this->socketfd_, 2048);
  assert(ret >= 0);
  return true;
}

int Socket::Accept(struct sockaddr_in& clientaddr) {
  socklen_t lengthofsockaddr = sizeof(clientaddr);
  int clientfd = accept(this->socketfd_, (struct sockaddr*)&clientaddr,
                        &lengthofsockaddr);
  if (clientfd < 0) {
    // nonblocking and epoll edge, errno == EAGAINx
    return clientfd;
  }
  return clientfd;
}

bool Socket::Close() {
  close(this->socketfd_);
  return true;
}
