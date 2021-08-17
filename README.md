# A C++ High Performance WebServer
based on epoll edge trigger, nonblocking IO, IO & worker thread pool 

- [x] single thread echo server (v1.0.0)
- [x] single thread http(get) server (v1.0.0)
- [x] eventloop pool echo server (v2.0.0)
- [x] eventloop pool http server (v2.0.0)
- [x] eventloop pool(io thread) & worker thread Pool echo server (v3.0.0)
- [x] eventloop pool(io thread) & worker thread Pool http server (v3.0.0)

## Build
### echo server
```
$ make echoserver

--------------another terminal window--------------------
$ telnet 127.0.0.1 8088


```
### http server
- support method: GET  
- support Connection: keep-alive
- 为了让webbench正常收到FIN，当httpserver在短连接时，发送完完整的response就close掉连接
```
$ make httpserver
```

## Format
need clang-format
```
$ make format
```

## Performance Test
[WebBench](https://github.com/EZLippi/WebBench)  
[Web性能压力测试之Webbench使用详解](https://www.cnblogs.com/fjping0606/p/5852049.html)
```
$ make httpserver

--------------another terminal window--------------------
$ webbench -c 20 -t 30 --get -1 http://127.0.0.1:8088/index.html

$ webbench -c 20 -t 30 --get -1 http://127.0.0.1:8088/webbench

```

## Issue
-  ~~HTTP parser state machine~~ 直到session存储整个完整的请求头才解析回复  
- 单线程->iothread pool + 单工作线程-> iothread pool + worker thread pool  发现性能吞吐量越来越低(环境：2核虚拟机ubuntu20.04)

## Ref
[chenshuo(muduo)](https://github.com/chenshuo/muduo)  
[linyacool(WebServer)](https://github.com/linyacool/WebServer)  
[chenshuaihao(NetServer)](https://github.com/chenshuaihao/NetServer)  
[jayice(NetServre)](https://github.com/Jayice-zjw/WebServer)  
《linux多线程服务端编程》  
《Linux高性能服务器编程》
