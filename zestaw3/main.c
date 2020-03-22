//
// Created by Dima Zhylko on 20/03/2020.
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
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

int read_from_file(FILE* f, char** dest){
    size_t size = 0;
    size_t l = getline(dest, &size, f);
    if(l == 0){
        printf("Can't read line from file, EOF reached\n");
        return 1;
    }

    (*dest)[size-2] = '\0';
    return 0;
}

int read_paths(char* lists_path, char** path_to_A, char** path_to_B, char** path_to_C){
    FILE* f = fopen(lists_path, "r");
    if(f == NULL){
        printf("Errno %d occurred while opening file %s\n", errno, lists_path);
        return 1;
    }

    if(read_from_file(f, path_to_A) != 0){
        return 1;
    }

    if(read_from_file(f, path_to_B) != 0){
        free(path_to_A);
        return 1;
    }

    if(read_from_file(f, path_to_C) != 0){
        free(path_to_A);
        free(path_to_B);
        return 1;
    }

    if(access(*path_to_A, F_OK | R_OK) == -1 || access(*path_to_B, F_OK | R_OK) == -1){
        printf("Can't read specified files or files doesn't exist\n");
        free(path_to_A);
        free(path_to_B);
        free(path_to_C);
        return 1;
    }
    return 0;
}

int preprocess_file(char* path, int* row, int* col){
    FILE* f = fopen(path, "r");
    if(f == NULL){
        printf("Errno %d occurred while opening file %s\n", errno, path);
        return 1;
    }

    char* buf = NULL;
    size_t size = 0;

    while(getline(&buf, &size, f) != -1){
        (*row)++;
        if(*row == 1){
            char* t[20];
            int offset = 0;

            while(sscanf(buf+offset, "%s", t) != EOF){
                offset += (strlen(t) + 1) * sizeof(char);
                (*col)++;
            }
        }
    }

    return 0;
}

int main(int argc, char **argv) {

    if(argc != 5){
        printf("Wrong number of arguments\n");
        exit(1);
    }

    int worker_count = atoi(argv[2]);
    //useconds_t timeout = strtol(argv[3], NULL, 10) * 1000000;
    pid_t worker_pool[worker_count];

    char* path_to_A = NULL;
    char* path_to_B = NULL;
    char* path_to_C = NULL;

    if(read_paths(argv[1], &path_to_A, &path_to_B, &path_to_C) != 0){
        exit(1);
    }

    // preprocessing to find size of matrix
    int row_A = 0, col_A = 0;
    preprocess_file(path_to_A, &row_A, &col_A);

    // TODO?: transpose matrix B
    int row_B = 0, col_B = 0;
    preprocess_file(path_to_B, &row_B, &col_B);

    int len = snprintf(NULL, 0, "%d", worker_count) + 1;
    char stride[len];
    snprintf(stride, len, "%d", worker_count);

    // create communication file
    // first worker_count lines tell each process if it's should be terminated (time out reached)
    // next line tells what was the last column written to file
    FILE* f = fopen("communication.tmp", "w+");

    flock(fileno(f), LOCK_EX);

    for (int i = 0; i < worker_count; i++) {
        fwrite("1\n", sizeof(char), 2, f);
        if((worker_pool[i] = fork()) == 0){
            // TODO: refactor
            len = snprintf(NULL, 0, "%d", i)+1;
            char cidx[len];
            snprintf(cidx, len, "%d", i);

            len = snprintf(NULL, 0, "%d", row_A)+1;
            char crow_A[len];
            snprintf(crow_A, len, "%d", row_A);

            len = snprintf(NULL, 0, "%d", col_A)+1;
            char ccol_A[len];
            snprintf(ccol_A, len, "%d", col_A);

            len = snprintf(NULL, 0, "%d", row_B)+1;
            char crow_B[len];
            snprintf(crow_B, len, "%d", row_B);

            len = snprintf(NULL, 0, "%d", col_B)+1;
            char ccol_B[len];
            snprintf(ccol_B, len, "%d", col_B);
            // refactor

            execl("./child", "./child", cidx, stride, "communication.tmp", path_to_A, crow_A, ccol_A, path_to_B, crow_B, ccol_B, path_to_C, "0", NULL);
            exit(0);
        }
    }

    fwrite("-1\n", sizeof(char), 3, f);
    fflush(f);
    flock(fileno(f), LOCK_UN);

    int status = 0;
    int timeout = atoi(argv[3]) * 1000000;
    int time = 0;
    int sleep_time = 20000; //20 ms

    for(int i = 0;i<worker_count;i++){
        while(time < timeout){
            waitpid(worker_pool[i], &status, WNOHANG);
            if(status != 0){
                printf("Process with PID %d performed %d multiplications\n", worker_pool[i], WEXITSTATUS(status));
                break;
            } else {
                usleep(sleep_time);
                time += sleep_time;
            }
        }
        if(time >= timeout){
            flock(fileno(f), LOCK_EX);
            fseek(f, 2*i*sizeof(char), SEEK_SET);
            fprintf(f, "0\n");
            fflush(f);
            flock(fileno(f), LOCK_UN);
            waitpid(worker_pool[i], &status, 0);
            printf("Process with PID %d performed %d multiplications\n", worker_pool[i], WEXITSTATUS(status));
        }
    }
    printf("Main Process ends\n");

    return 0;
}