# A C++ High Performance WebServer
based on epoll edge trigger, nonblocking IO

- [x] single thread echo server
- [x] single thread http(get) server

## Build
### single thread echo server
```
$ make echoserver

--------------another terminal window--------------------
$ telnet 127.0.0.1 8088


```
### single thread echo server
support method: GET  
support Connection: keep-alive
```
$ make httpserver
```

## format
```
$ make format
```

## Ref
[linyacool(WebServer)](https://github.com/linyacool/WebServer)  
[chenshuaihao(NetServer)](https://github.com/chenshuaihao/NetServer)  
[jayice(NetServre)](https://github.com/Jayice-zjw/WebServer)  
《linux多线程服务端编程》  
《Linux高性能服务器编程》
