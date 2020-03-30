//
// Created by Dima Zhylko on 30/03/2020.
//
#include "defines.h"

int main(int argc, char** argv){ // "./child", sig, option
    int sig = atoi(argv[1]);
    char* option = argv[2];

    signal(sig, handler);
    prefix = "Exec process";
    if(equals(option, "p"))RAPORT_PENDING(sig)
    else {
        raise(sig);
    }
    fflush(stdout);
    exit(0);
}