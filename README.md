# CppServer

-cpp服务器开发中

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
