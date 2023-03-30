#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* 监视标准输入（文件描述符0）是否可读 */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* 设置超时时间为5秒 */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    /* 等待标准输入可读或超时 */
    retval = select(1, &rfds, NULL, NULL, &tv);

    /* 检查select()函数的返回值 */
    if (retval == -1) {
        perror("select()");
    } else if (retval) {
        printf("Data is available now.\n");
    } else {
        printf("No data within five seconds.\n");
    }

    return 0;
}
