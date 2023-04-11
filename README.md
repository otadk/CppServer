# CppServer

这是一个小型的用于网络聊天的程序，分为服务端与客户端。

通过编译server.c获得服务端程序，通过编译client.c获得客服端程序，另外通过Qt获得客户端GUI程序。

编译server.c只需要server.h头文件。

编译client.c只需要client.c头文件。

```bash
gcc server.c -D _D -o server #生成server可执行程序
gcc client.c -o client #生成client可执行程序
```

Qt需要使用Qt开发工具在载入Qt目录工程，进行编译，这里的可执行文件只适用于作者的机器。

common和test目录是开发过程中的代码，并不用于最终编译。

### 功能简介

本程序代码量不大，只是实现了聊天系统最基础的功能，登陆，登出与发消息。

要运行本程序首先要启动server服务端程序（简称S），开放确定的端口（默认7788，可以更改程序中port的值更改端口号）。

S会创建socket套接字监听网络连接，并创建线程池用于处理事件，然后不断等待终端输入或网络连接输入，直到终端输入q表示程序结束。

启动client客户端程序（简称C）之后会向S发送网络套接字连接，并等待终端输入指令，C会根据输入指令向S发送消息，同时也会接受S发送的消息并显示出来。

S通过哈希实现了用户名判断，当一个新的用户使用已经被之前用户使用过的用户名登陆时，会失败。

当C发送消息时，所有用户都会收到消息。

### 未来想法

1 优化Qt使用逻辑。

2 服务端可以过一段时间发送一个在线用户数量的消息。

3 服务端可以通过终端输入发送消息

4 有什么潜在风险？如何提高安全性？

### commmon

head引用所有头文件

color提供颜色宏

common提供常用函数

hash提供哈希集合

thread_pool提供线程池（任务队列）

### test

编译test/epollEcho.c得到用epoll实现的echo服务器(返回发送的内容，将大小写转换)

编译test/socketTest.c得到用select实现的接收服务器(纯接收不处理)

test/colorTest.c为了测试颜色

test/FopenFile.c为了测试文件操作

test/selectTest.c为了测试select函数

test/pthreadPoolTest.c为了测试线程池是
