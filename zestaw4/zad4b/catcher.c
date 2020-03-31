//
// Created by Dima Zhylko on 30/03/2020.
//

#include "defines.h"

int main(int argc, char** argv){ // "./catcher", mode
    pid_t main = getppid();
    printf("Catcher PID: %d\n", getpid());

    SETUP_SIGEND
    SETUP_SIGCOUNT

    kill(main, SIGUSR2);
    fflush(stdout);

    while(receive);
    printf("Catcher received %d signals\n", counter);
    fflush(stdout);

    exit(0);
}