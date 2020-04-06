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

#define MAX_LEN 100

int main(int argc, char** argv){
    if(argc != 4){
        perror("Wrong number of arguments\n");
        exit(1);
    }

    int fd = open(argv[1], O_WRONLY);
    FILE* in = fopen(argv[2], "r");
    int len = atoi(argv[3]);

    char buf[MAX_LEN+1];
    char wbuf[MAX_LEN+10];

    srand(time(NULL));

    while(fread(buf, sizeof(char), len, in) != 0){
        sprintf(wbuf, "#%d#%s\n", getpid(), buf);
        write(fd, wbuf, strlen(wbuf));
        sleep(1+rand()%2);
    }

    close(fd);
    fclose(in);

    return 0;
}