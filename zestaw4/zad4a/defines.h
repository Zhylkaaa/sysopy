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

#define KILL "kill"
#define SIGQUEUE "sigqueue"
#define SIGRT "sigrt"

#define equals(s1, s2) strcmp(s1, s2) == 0
#define SIGCOUNT equals(mode, SIGRT) ? SIGRTMIN+1 : SIGUSR1
#define SIGEND equals(mode, SIGRT) ? SIGRTMIN+2 : SIGUSR2
#define SETUP_SIGNALS_AND_HANDLERS \
signal(SIGCOUNT, count);\
struct sigaction act = {.sa_flags = SA_SIGINFO, .sa_sigaction = end};\
sigaction(SIGEND, &act, NULL);
#define END_TRANSMISSION kill(target, SIGEND);
#define END_TRANSMISSION_QUEUE sigqueue(target, SIGEND, val);

int counter;
int receive = 1;
pid_t target;
char* mode;

union sigval val = {.sival_int=0};

void count(int sig){counter++;}

void end(int sig, siginfo_t* info, void *ucontext){
    target = info->si_pid;
    receive = 0;
    val.sival_int = info->si_value.sival_int;
}
#endif //SYSOPY_DEFINES_H
