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

    SETUP_SIGNALS_AND_HANDLERS

    for(int i = 0;i<num_sigs;i++)kill(target, SIGCOUNT);
    END_TRANSMISSION

    while(receive);

    if(!(equals(mode, SIGQUEUE))){
        printf("Sender received %d/%d signals\n", counter, num_sigs);
    } else {
        printf("Sender received %d signals, catcher sanded %d signals\n", counter, val.sival_int);
    }

    fflush(stdout);

    exit(0);
}
