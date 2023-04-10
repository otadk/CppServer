# CppServer

这是一个用于网络聊天的程序，分为服务端与客户端。

通过编译server.c获得服务端程序，通过编译client.c获得客服端程序，另外通过Qt获得客户端GUI程序。

编译server.c只需要server.h头文件。

编译client.c只需要client.c头文件。

```bash
gcc server.c -D _D -o server #生成server可执行程序
gcc client.c -o client #生成client可执行程序
```

编译client.c只需要client.c头文件。Qt需要使用Qt开发工具在Qt目录下进行编译。

common和test目录是开发过程中的代码，并不用于最终编译。

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
