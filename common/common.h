#ifndef _COMMON_H
#define _COMMON_H

int socket_create(int port);
int socket_connect(const char *ip, int port);
int make_block(int fd);
int make_nonblock(int fd);

#endif