#include "../common/head.h"

int main() {
    printf(YELLOW"<push> taskQueue is full.\n"NONE);
    DBG(YELLOW"<push> taskQueue is full.\n"NONE);
    printf(GREEN"<push> push task.\n"NONE);
    DBG(GREEN"<push> push task.\n"NONE);
    DBG(GREEN"<pop> pop task.\n"NONE);
    DBG(PINK"<thread> run.\n"NONE);
}