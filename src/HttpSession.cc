#include <iostream>
#include <stdio.h>
#include <string.h>
#include "HttpSession.h"

HttpSession::HttpSession() : statemsg_("OK"), statecode_(200) {}

HttpSession::~HttpSession() {}

void HttpSession::PraseHttpRequest(std::string& s) {
  std::string msg;
  msg.swap(s);
  std::string crlf("\r\n"), crlfcrlf("\r\n\r\n");
  size_t prev = 0, next = 0, pos_colon = 0;
  std::string key, value;

  // prase http request line
  if ((next = msg.find(crlf, prev)) != std::string::npos) {
    // GET http://xxx/ HTTP/1.1 \r\n
    std::string first_line(msg.substr(prev, next - prev));
    prev = next;
    std::stringstream sstream(first_line);
    sstream >> (this->reqctx_.method);
    sstream >> (this->reqctx_.url);
    sstream >> (this->reqctx_.version);
  } else {
    this->statecode_ = 400;
    this->statemsg_  = "Bad Request";
    return;
  }

  // parse http request header
  size_t pos_crlfcrlf = 0;
  if ((pos_crlfcrlf = msg.find(crlfcrlf, prev)) != std::string::npos) {
    while (prev != pos_crlfcrlf) {
      next      = msg.find(crlf, prev + 2);
      pos_colon = msg.find(":", prev + 2);
      key       = msg.substr(prev + 2, pos_colon - prev - 2);
      value     = msg.substr(pos_colon + 2, next - pos_colon - 2);
      prev      = next;
      this->reqctx_.header[key] = value;
    }
  } else {
    this->statecode_ = 400;
    this->statemsg_  = "Bad Request";
    return;
  }
  // parse http request body
  this->reqctx_.body = msg.substr(pos_crlfcrlf + 4);
}

void HttpSession::HttpProcess() {
  this->responsebody_.clear();
  this->respctx_.clear();
  if (this->statecode_ != 200) {
    this->HttpError(this->statecode_, this->statemsg_);
    return;
  }
  if ("GET" != this->reqctx_.method) {
    this->statecode_ = 501;
    this->statemsg_  = "Not Implemented";
    this->HttpError(this->statecode_, this->statemsg_);
    return;
  }

  // path
  size_t pos = this->reqctx_.url.find("?");
  if (pos != std::string::npos) {
    path_        = this->reqctx_.url.substr(0, pos);
    querystring_ = this->reqctx_.url.substr(pos + 1);
  } else {
    path_ = this->reqctx_.url;
  }

  // keepalive
  if (this->reqctx_.header.count("Connection") != 0) {
    this->keepalive_ =
        (this->reqctx_.header["Connection"] == "keep-alive");
  } else {
    if ("HTTP/1.1" == this->reqctx_.version)
      this->keepalive_ = true;
    // HTTP/1.0 默认短连接
  }

  if ("/" == this->path_)
    this->path_ = "/index.html";
  this->path_.insert(0, ".");
  FILE* fp = NULL;
  if ((fp = fopen(this->path_.c_str(), "rb")) == NULL) {
    this->statecode_ = 404;
    this->statemsg_  = "Not Found";
    this->HttpError(this->statecode_, this->statemsg_);
    return;
  } else {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    while (fread(buffer, sizeof(buffer), 1, fp) == 1) {
      this->responsebody_.append(buffer);
      memset(buffer, 0, sizeof(buffer));
    }
    if (feof(fp)) {
      this->responsebody_.append(buffer);
    } else {
      std::cout << "error fread" << std::endl;
      this->statecode_ = 500;
      this->statemsg_  = "Internl Server Error";
      this->HttpError(this->statecode_, this->statemsg_);
      return;
    }
    fclose(fp);
  }

  std::string filetype("text/html");
  this->respctx_ += this->reqctx_.version + " 200 OK\r\n";
  if (this->keepalive_)
    this->respctx_ += "Connection: keep-alive\r\n";
  // this->respctx_ += "Server: Wangqi WebServer\r\n";
  this->respctx_ += "Content-Type: text/html; charset=UTF-8\r\n";
  this->respctx_ +=
      "Content-Length: " + std::to_string(this->responsebody_.size()) +
      "\r\n";
  this->respctx_ += "\r\n";
  this->respctx_ += this->responsebody_;
}

void HttpSession::AddToBuf(std::string& s) { this->respctx_.swap(s); }

void HttpSession::HttpError(int err_num, std::string short_msg) {
  this->responsebody_ += "<html><title>Error</title>";
  this->responsebody_ += "<head><meta http-equiv=\"Content-Type\" "
                         "content=\"text/html; charset=utf-8\"></head>";
  this->responsebody_ +=
      "<style>body{background-color:#f;font-size:14px;}h1{"
      "font-size:60px;color:#eeetext-align:center;padding-"
      "top:30px;font-weight:normal;}</style>";
  this->responsebody_ += "<body bgcolor=\"ffffff\"><h1>";
  this->responsebody_ += std::to_string(err_num) + " " + short_msg;
  this->responsebody_ +=
      "</h1><hr><em> Wang Qi's WebServer</em>\n</body></html>";

  this->respctx_ += this->reqctx_.version + " " +
                    std::to_string(err_num) + " " + short_msg + "\r\n";
  this->respctx_ += "Content-Type: text/html\r\n";
  this->respctx_ +=
      "Content-Length: " + std::to_string(this->responsebody_.size()) +
      "\r\n";
  this->respctx_ += "\r\n";
  this->respctx_ += responsebody_;
}
