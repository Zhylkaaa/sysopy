//
// Created by Dima Zhylko on 29/03/2020.
//
#define _XOPEN_SOURCE 500
#include <ftw.h>
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

int f(const char *fpath, const struct stat *sb, int typeflag){
    printf("%s\n", fpath);
    return 0;
}

void handle_int(int signo){
    printf("Odebrano sygnał SIGINT\n");
    exit(0);
}

int waiting = 0;
sigset_t wait_set;

void handle_tstp(int signo){
    printf("Odebrano sygnał SIGTSTP\n");
    if(waiting == 0){
        waiting = 1;
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
        sigsuspend(&wait_set);
    } else {
        waiting = 0;
    }
}

int main(){
    struct sigaction sa;
    sa.sa_handler = handle_tstp;
    sigemptyset(&sa.sa_mask);
    sigfillset(&wait_set);
    sigdelset(&wait_set, SIGTSTP);
    sigdelset(&wait_set, SIGINT);

    sa.sa_flags = 0;

    while(1){
        sigaction(SIGTSTP, &sa, NULL);
        signal(SIGINT, handle_int);
        ftw(".", f, 1);
        printf("\n");
        sleep(1);
    }
    return 0;
}