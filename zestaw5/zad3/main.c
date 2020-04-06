//
// Created by Dima Zhylko on 06/04/2020.
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

int main(int argc, char** argv){
    if(argc != 9){
        perror("wrong number of arguments\n");
        exit(1);
    }

    if(mkfifo(argv[1], 0666) != 0){
        perror("Error while creating FIFO\n");
        exit(1);
    }

    pid_t pids[6];

    if((pids[0] = fork()) == 0){
        execl("./consumer", "./consumer", argv[1], argv[8], NULL);
    }

    for(int i = 0;i<5;i++){
        if((pids[1+i] = fork()) == 0){
            execl("./producer", "./producer", argv[1], argv[2+i], argv[7], NULL);
        }
    }

    for(int i = 0;i<6;i++){
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
