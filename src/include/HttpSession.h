#ifndef _HTTP_SESSION_H_
#define _HTTP_SESSION_H_

#include <string>
#include <sstream>
#include <map>

enum class STATECODE {
  OK                    = 200,
  BAD_REQUEST           = 400,
  NOTFOUND              = 404,
  INTERNEL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED       = 501
};

typedef struct _HttpRequestContext {
  std::string method;
  std::string url;
  std::string version;
  std::map<std::string, std::string> header;
  std::string body;
} HttpRequestContext;

typedef struct _HttpResponseContext {
  std::string version;
  std::string statecode;
  std::string statemsg;
  std::map<std::string, std::string> header;
  std::string body;
} HttpResponseContext;

class HttpSession {
private:
  HttpRequestContext reqctx_;

  bool keepalive_;
  int statecode_;
  std::string statemsg_;
  std::string path_;
  std::string querystring_;
  std::string respctx_;
  std::string responsebody_;
  std::string body_buff_;

public:
  HttpSession();
  ~HttpSession();

  void PraseHttpRequest(std::string& s);
  void HttpProcess();
  void AddToBuf(std::string& s);
  void HttpError(int err_num, std::string short_msg);
  bool KeepAlive() { return this->keepalive_; }
};

#endif
