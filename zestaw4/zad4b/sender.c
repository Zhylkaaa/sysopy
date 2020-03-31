//
// Created by Dima Zhylko on 30/03/2020.
//

#include "defines.h"

int main(int argc, char** argv){ // "./sender", num_sigs, catcher_pid, mode

    printf("Sender PID %d\n", getpid());
    fflush(stdout);

    int num_sigs = atoi(argv[1]);
    target = (pid_t) atoi(argv[2]);
    char* mode = argv[3];

    SETUP_SIGEND
    signal(SIGCOUNT, do_nothing);

    for(int i = 0;i<num_sigs;i++)SEND_AND_WAIT(target, SIGCOUNT)
    END_TRANSMISSION

    fflush(stdout);

    exit(0);
}
