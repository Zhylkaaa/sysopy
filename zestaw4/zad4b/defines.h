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

#define equals(s1, s2) strcmp(s1, s2) == 0
#define SIGCOUNT SIGUSR1
#define SIGEND SIGUSR2
#define SETUP_SIGEND \
struct sigaction act = {.sa_flags = SA_SIGINFO, .sa_sigaction = end};\
sigaction(SIGEND, &act, NULL);
#define SETUP_SIGCOUNT \
act.sa_sigaction = count;\
sigaction(SIGCOUNT, &act, NULL);

#define END_TRANSMISSION kill(target, SIGEND);
#define SEND_AND_WAIT(target, sig){\
    kill(target, SIGCOUNT);\
    sigset_t wait_set;\
    sigfillset(&wait_set);\
    sigdelset(&wait_set, SIGCOUNT);\
    signal(SIGUSR2, do_nothing); \
    sigsuspend(&wait_set);\
}

int counter;
int receive = 1;
pid_t target;

union sigval val = {.sival_int=0};

void count(int sig, siginfo_t* info, void *ucontext){
    counter++;
    kill(info->si_pid, SIGCOUNT);
}

void do_nothing(int sig){}

void end(int sig, siginfo_t* info, void *ucontext){
    target = info->si_pid;
    receive = 0;
    val.sival_int = info->si_value.sival_int;
}
#endif //SYSOPY_DEFINES_H
