#ifndef _CHAT_H
#define _CHAT_H

#define CHAT_SIGNUP  1
#define CHAT_SIGNIN  2
#define CHAT_SIGNOUT 3
#define CHAT_ACK     4
#define CHAT_NAK     5
#define CHAT_SENDMSG 6

struct chat_msg {
    int type;
    int sex;
    char name[20];
    char passwd[20];
    char msg[512];
};

#endif