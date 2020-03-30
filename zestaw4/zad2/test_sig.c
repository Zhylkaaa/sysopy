//
// Created by Dima Zhylko on 30/03/2020.
//
#include "defines.h"

HANDLE_OPTION(i){
    signal(sig, SIG_IGN);
    raise(sig);
    FORK_CHILD_DO{
        if(equals(mode, EXEC))EXEC_WITH(sig, "i")
        else raise(sig);
        exit(0);
    }
}

HANDLE_OPTION(h){
    raise(sig);
    FORK_CHILD_DO{
        prefix = "Forked process";
        raise(sig);
        exit(0);
    }
    waitpid(WAIT_ANY, NULL, 0);
}

HANDLE_OPTION(m){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    sigprocmask(SIG_SETMASK, &set, NULL);

    raise(sig);
    FORK_CHILD_DO{
        prefix = "Forked process";
        if(equals(mode, EXEC))EXEC_WITH(sig, "m")
        else raise(sig);
        fflush(stdout);
    }
    fflush(stdout);
    waitpid(WAIT_ANY, NULL, 0);
}

HANDLE_OPTION(p){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    sigprocmask(SIG_SETMASK, &set, NULL);

    raise(sig);
    RAPORT_PENDING(sig);
    FORK_CHILD_DO{
        prefix = "Forked process";
        if(equals(mode, EXEC)){
            raise(sig);
            EXEC_WITH(sig, "p")
            exit(0);
        }
        RAPORT_PENDING(sig);
        exit(0);
    }
    waitpid(WAIT_ANY, NULL, 0);
}

int main(int argc, char** argv){ // ./test_sig, sig, option, mode
    int sig = atoi(argv[1]);
    char* option = argv[2];
    char* mode = argv[3];

    prefix = "Main process";

    SETUP_HANDLER

    printf("sig: %s\n", argv[1]);
    fflush(stdout);

    if(equals(option, "i"))handle_i(sig, mode);
    else if(equals(option, "h"))handle_h(sig, mode);
    else if(equals(option, "m"))handle_m(sig, mode);
    else handle_p(sig, mode);
}