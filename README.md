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

## performance test
[WebBench](https://github.com/EZLippi/WebBench)  
[Web性能压力测试之Webbench使用详解](https://www.cnblogs.com/fjping0606/p/5852049.html)
```
$ make httpserver

--------------another terminal window--------------------
$ webbench -c 20 -t 30 --get -1 http://127.0.0.1:8088/index.html

$ webbench -c 20 -t 30 --get -1 http://127.0.0.1:8088/webbench

```

## BUG
HTTP parser state machine

## Ref
[chenshuo(muduo)](https://github.com/chenshuo/muduo)
[linyacool(WebServer)](https://github.com/linyacool/WebServer)  
[chenshuaihao(NetServer)](https://github.com/chenshuaihao/NetServer)  
[jayice(NetServre)](https://github.com/Jayice-zjw/WebServer)  
《linux多线程服务端编程》  
《Linux高性能服务器编程》
