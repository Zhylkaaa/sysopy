//
// Created by Dima Zhylko on 31/03/2020.
//

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


void handle_segv(int sig, siginfo_t* info, void* ucontext){
    printf("si_addr %p\n", info->si_addr);
    exit(0);
}

void handle_int(int sig, siginfo_t* info, void* ucontext){
    printf("si_uid %u\n", info->si_uid);
    exit(0);
}

void handle_fpe(int sig, siginfo_t* info, void* ucontext) {
    printf("si_code %u\n", info->si_code);
    exit(0);
}

int main() {
    int a = 0;
    if(fork() == 0){
        struct sigaction act = {.sa_flags=SA_SIGINFO, .sa_sigaction=handle_fpe};
        sigaction(SIGFPE, &act, NULL);

        int b = 1 / a;
    }
    wait(NULL);

    if(fork() == 0){
        struct sigaction act = {.sa_flags=SA_SIGINFO, .sa_sigaction=handle_segv};
        sigaction(SIGSEGV, &act, NULL);
        // causes segmentation fault
        int* c = (int*) 12345;
        *c = 213;
    }
    wait(NULL);

    if(fork() == 0){
        struct sigaction act = {.sa_flags=SA_SIGINFO, .sa_sigaction=handle_int};
        sigaction(SIGINT, &act, NULL);
        raise(SIGINT);
    }
    wait(NULL);
}