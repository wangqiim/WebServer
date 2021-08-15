#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include "TcpConnection.h"

#define BUFSIZE 4096

int recvn(int fd, std::string& bufferin);
int sendn(int fd, std::string& bufferout);

TcpConnection::TcpConnection(EventLoop* loop, int fd,
                             struct sockaddr_in clientaddr)
    : loop_(loop), fd_(fd), clientaddr_(clientaddr), halfclose_(false) {
  this->channel_ = new Channel();
  this->channel_->SetFd(this->fd_);
  this->channel_->SetEvents(EPOLLIN | EPOLLET);
  this->channel_->SetReadHandle(
      std::bind(&TcpConnection::HandleRead, this));
  this->channel_->SetWriteHandle(
      std::bind(&TcpConnection::HandleWrite, this));
  this->channel_->SetCloseHandle(
      std::bind(&TcpConnection::HandleClose, this));
  this->channel_->SetErrorHandle(
      std::bind(&TcpConnection::HandleError, this));

  // this->loop_->AddChannelToPoller(this->channel_);
}

TcpConnection::~TcpConnection() {
  this->loop_->RemoveChannelToPoller(this->channel_);
  delete this->channel_;
  close(this->fd_);
}

void TcpConnection::Send(std::string& msg) {
  this->bufferout_ += msg;
  int result = sendn(this->fd_, this->bufferout_);
  if (result > 0) {
    uint32_t events = this->channel_->GetEvents();
    if (this->bufferout_.size() > 0) {
      // send buf is not empty, need resend
      this->channel_->SetEvents(events | EPOLLOUT);
      this->loop_->UpdateChannelToPoller(this->channel_);
    } else {
      this->channel_->SetEvents(events & (~EPOLLOUT));
      this->sendcompletecallback_(this);
      if (this->halfclose_)
        this->HandleClose();
    }
  } else if (result < 0) {
    HandleError();
  } else {
    HandleClose();
  }
}

void TcpConnection::HandleRead() {
  int res = recvn(this->fd_, this->bufferin_);
  if (res > 0) {
    this->messagecallback_(this, this->bufferin_);
  } else if (res == 0) {
    // fin
    HandleClose();
  } else {
    HandleError();
  }
}

void TcpConnection::HandleWrite() {
  int res = sendn(this->fd_, this->bufferout_);
  if (res > 0) {
    uint32_t events = this->channel_->GetEvents();
    if (bufferout_.size() > 0) {
      this->channel_->SetEvents(events | EPOLLOUT);
      this->loop_->UpdateChannelToPoller(this->channel_);
    } else {
      // send all msg
      this->channel_->SetEvents(events & (~EPOLLOUT));
      this->sendcompletecallback_(this);
      if (this->halfclose_)
        this->HandleClose();
    }
  } else if (res < 0) {
    HandleError();
  } else {
    HandleClose();
  }
}

void TcpConnection::HandleError() {
  this->loop_->AddTask(this->connectioncleanup_);
  this->errorcallback_(this);
}

//  Elegant closed
void TcpConnection::HandleClose() {
  if (this->bufferout_.size() > 0) {
    this->halfclose_ = true;
  } else {
    this->loop_->AddTask(this->connectioncleanup_);
    this->closecallback_(this);
  }
}

int recvn(int fd, std::string& bufferin) {
  int nbyte   = 0;
  int readsum = 0;
  char buffer[BUFSIZE];
  for (;;) {
    nbyte = read(fd, buffer, BUFSIZE);

    if (nbyte > 0) {
      bufferin.append(buffer, nbyte);
      readsum += nbyte;
      if (nbyte < BUFSIZE)
        return readsum;
      else
        continue;
    } else if (nbyte < 0) {
      if (errno == EAGAIN) {
        return readsum;
      } else if (errno == EINTR) {
        std::cout << "errno == EINTR" << std::endl;
        continue;
      } else {
        perror("recv error");
        std::cout << "recv error" << std::endl;
        return -1;
      }
    } else { // FIN
      // std::cout << "client close the Socket" << std::endl;
      return 0;
    }
  }
}

int sendn(int fd, std::string& bufferout) {
  ssize_t nbyte = 0;
  int sendsum   = 0;
  char buffer[BUFSIZE + 1];
  size_t length = bufferout.size();

  if (length >= BUFSIZE)
    length = BUFSIZE;

  for (;;) {
    nbyte = write(fd, bufferout.c_str(), length);
    if (nbyte > 0) {
      sendsum += nbyte;
      bufferout.erase(0, nbyte);
      length = bufferout.size();
      if (length >= BUFSIZE)
        length = BUFSIZE;
      if (length == 0)
        return sendsum;
    } else if (nbyte < 0) {
      if (errno == EAGAIN) { // nonblock
        std::cout << "write errno == EAGAIN,not finish!" << std::endl;
        return sendsum;
      } else if (errno == EINTR) {
        std::cout << "write errno == EINTR" << std::endl;
        continue;
      } else if (errno == EPIPE) {
        perror("write error");
        std::cout << "write errno == client send RST" << std::endl;
        return -1;
      } else {
        perror("write error"); // Connection reset by peer
        std::cout << "write error, unknow error" << std::endl;
        return -1;
      }
    } else {
      perror("unexpected happen");
      return -1;
    }
  }
}