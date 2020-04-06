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

#define MAX_LEN 1024

int main(int argc, char** argv){
    if(argc != 2){
        perror("Something went wrong, mate");
        exit(1);
    }

    FILE* f = fopen(argv[1], "r");
    if(f == NULL){
        perror("Error opening file");
        exit(1);
    }

    FILE* sort_in = popen("sort", "w");
    char buf[MAX_LEN];

    while(fgets(buf, MAX_LEN, f) != NULL){
        fputs(buf, sort_in);
    }

    pclose(sort_in);

    return 0;
}