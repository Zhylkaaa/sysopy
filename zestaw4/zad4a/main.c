//
// Created by Dima Zhylko on 30/03/2020.
//

#include "defines.h"
#define exec(var, program, args...) if((var = fork()) == 0)execl(program, program, args, NULL)

void do_nothing(int sig){}

int main(int argc, char** argv){ // "./main", num_sigs, mode
    pid_t catcher;
    exec(catcher, "./catcher", argv[2]);
    sigset_t wait_set;
    sigfillset(&wait_set);
    sigdelset(&wait_set, SIGUSR2);
    signal(SIGUSR2, do_nothing);
    sigsuspend(&wait_set);

    static char catcher_pid[10];
    sprintf(&catcher_pid, "%d", catcher);
    pid_t sender;
    exec(sender, "./sender", argv[1], catcher_pid, argv[2]);

    waitpid(catcher, NULL);
    waitpid(sender, NULL);

    exit(0);
}
