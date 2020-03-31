//
// Created by Dima Zhylko on 30/03/2020.
//

#include "defines.h"

int main(int argc, char** argv){ // "./catcher", mode
    mode = argv[1];
    pid_t main = getppid();
    printf("Catcher PID: %d\n", getpid());

    SETUP_SIGNALS_AND_HANDLERS
    kill(main, SIGUSR2);
    fflush(stdout);

    while(receive);
    printf("Catcher received %d signals\n", counter);
    fflush(stdout);

    for(int i = 0;i<counter;i++)kill(target, SIGCOUNT);

    if(!(equals(mode, SIGQUEUE))){
        END_TRANSMISSION
    } else {
        val.sival_int = counter;
        END_TRANSMISSION_QUEUE
    }

    exit(0);
}