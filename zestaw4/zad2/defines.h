//
// Created by Dima Zhylko on 30/03/2020.
//

#ifndef SYSOPY_DEFINES_H
#define SYSOPY_DEFINES_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include <signal.h>

#define FORK "0"
#define EXEC "1"
#define SEPARATOR printf("-------------------------------------------------------------------\n")
#define exec(program, args...) execl(program, program, args, NULL) // credit to github.com/Pan-Maciek
#define equals(s1, s2) strcmp(s1, s2) == 0
#define SETUP_HANDLER {\
    for(int i = 1;i<31;i++){\
        signal(i, handler);\
    }\
}
#define FORK_CHILD_DO if(fork() == 0)
#define HANDLE_OPTION(option) void handle_##option(int sig, char* mode)
#define RAPORT_PENDING(sig) {\
    sigset_t mask;\
    sigpending(&mask);\
    if(sigismember(&mask, sig))printf("%s have pending signal %d\n", prefix, sig); \
    else printf("%s doesn't have pending signal %d\n", prefix, sig);\
    fflush(stdout); \
}
#define EXEC_TEST(sig, ssig, mode, option){\
    static char ssig[3]; \
    sprintf(&ssig, "%d", sig); \
    if(fork() == 0) execl("./test_sig", "./test_sig", ssig, option, mode, NULL);\
    waitpid(WAIT_ANY, NULL, 0);\
}
#define EXEC_WITH(sig, option) {\
    static char ssig[3]; \
    sprintf(&ssig, "%d", sig); \
    exec("./child", ssig, option);\
}
char* prefix;

void handler(int signo){
    printf("%s got signal %d\n", prefix, signo);
    fflush(stdout);
}

#endif //SYSOPY_DEFINES_H
