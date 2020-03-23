//
// Created by Dima Zhylko on 23/03/2020.
//

#define _XOPEN_SOURCE 500
#include <ftw.h>
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

int f(const char *fpath, const struct stat *sb, int typeflag){
    if(typeflag == FTW_D){
        printf("%s/\n", fpath);
        pid_t p;
        if((p = fork()) == 0){
            execl("/bin/ls", "ls", "-l", fpath, NULL);
            exit(0);
        }
        int status;
        waitpid(p, &status, 0);
    }
    return 0;
}

int main(int argc, char** argv){
    if(argc < 2){
        printf("Specify dir to list\n");
        exit(1);
    }

    int max_depth = 50;

    if(argc == 4){
        if(strcmp(argv[2], "-mdepth") != 0){
            printf("Wrong argument %s\n", argv[2]);
        } else {
            max_depth = atoi(argv[3]);
        }
    }

    ftw(argv[1], f, max_depth);

    return 0;
}