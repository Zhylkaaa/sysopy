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
    if(argc != 3){
        perror("Wrong number of arguments\n");
        exit(1);
    }

    FILE* in = fopen(argv[1], "r");
    FILE* out = fopen(argv[2], "w");

    char buf[MAX_LEN+1];

    int tm;
    while(fscanf(in, "#%d#%s\n", &tm, buf) != EOF){
        fprintf(out, "%s\n", buf);
        printf("%s\n", buf);
    }

    fclose(in);
    fclose(out);

    return 0;
}