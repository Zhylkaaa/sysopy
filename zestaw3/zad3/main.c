//
// Created by Dima Zhylko on 20/03/2020.
//

#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

int read_from_file(FILE* f, char** dest){
    *dest = calloc(100, sizeof(char));
    if(fscanf(f, "%s\n", *dest) == 0){
        printf("Can't read line from file, EOF reached\n");
        return 1;
    }
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

    /*if(access(*path_to_A, F_OK | R_OK) == -1 || access(*path_to_B, F_OK | R_OK) == -1){
        printf("Can't read specified files or files doesn't exist\n");
        free(path_to_A);
        free(path_to_B);
        free(path_to_C);
        return 1;
    }*/
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
            char t[20];
            int offset = 0;

            while(sscanf(buf+offset, "%s", t) != EOF){
                offset += (strlen(t) + 1) * sizeof(char);
                (*col)++;
            }
        }
    }

    return 0;
}

int set_limits(char* t, char* mem){
    int time_limit = atoi(t);
    struct rlimit r_limit_cpu_time;
    r_limit_cpu_time.rlim_cur = time_limit;
    r_limit_cpu_time.rlim_max = time_limit;

    if (setrlimit(RLIMIT_CPU, &r_limit_cpu_time) == -1) {
        printf("Can't set limit to CPU time\n");
        printf("Errno %d\n", errno);
        return 1;
    }

    long int mem_limit = strtol(mem, NULL, 10) * 1024 * 1024;

    struct rlimit r_mem_limit;
    r_mem_limit.rlim_cur = mem_limit;
    r_mem_limit.rlim_max = mem_limit;

    if (setrlimit(RLIMIT_AS, &r_mem_limit) == -1) {
        printf("Can't set memory limit\n");
        printf("Errno %d\n", errno);
        return 1;
    }

    return 0;
}

void get_time_usage(struct timeval* user_time, struct timeval* sys_time){
    struct rusage usage;

    getrusage(RUSAGE_CHILDREN, &usage);

    *user_time = usage.ru_utime;
    *sys_time = usage.ru_stime;
}

int main(int argc, char **argv) {

    if(argc != 7){
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

    //printf("%s %s %s\n", path_to_A, path_to_B, path_to_C);

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

    char* mode = argv[4];

    if(strcmp(mode, "1") != 0 && strcmp(mode, "0")){
        printf("Wrong mode\n");
        exit(1);
    }

    if(strcmp(mode, "0") == 0){
        FILE* tm = fopen(path_to_C, "w+");
        fclose(tm);
    }

    for (int i = 0; i < worker_count; i++) {
        fwrite("1\n", sizeof(char), 2, f);
        if((worker_pool[i] = fork()) == 0){
            if(set_limits(argv[5], argv[6]) != 0){
                printf("Error setting limits\n");
                exit(0);
            }

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

            execl("./child", "./child", cidx, stride, "communication.tmp", path_to_A, crow_A, ccol_A, path_to_B, crow_B, ccol_B, path_to_C, mode, NULL);
        }
    }

    fwrite("-1\n", sizeof(char), 3, f);
    fflush(f);
    flock(fileno(f), LOCK_UN);

    int status = 0;
    int timeout = atoi(argv[3]) * 1000000;
    int time = 0;
    int sleep_time = 20000; //20 ms
    int num_files = 0;

    for(int i = 0;i<worker_count;i++){
        while(time < timeout){
            if(waitpid(worker_pool[i], &status, WNOHANG) != 0){
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
            printf("Process with PID %d performed %d multiplications before being killed\n", worker_pool[i], WEXITSTATUS(status));
        }

        struct timeval user_time;
        struct timeval sys_time;

        get_time_usage(&user_time, &sys_time);

        printf("Process PID %d report:\nUser CPU time: %ld s %d us\nSystem CPU time: %ld s %d us\n", worker_pool[i], user_time.tv_sec, user_time.tv_usec, sys_time.tv_sec, sys_time.tv_usec);
        num_files += WEXITSTATUS(status);
    }

    free(path_to_A);
    free(path_to_B);

    if(strcmp(mode, "1") == 0){
        FILE* o = fopen(path_to_C, "w+");
        if(fork() == 0){
            char** params = (char**) calloc(num_files+4, sizeof(char*));
            params[0] = "/usr/bin/paste"; // only macOS path?
            params[1] = "-d";
            params[2] = " ";

            if(o == NULL){
                printf("Error opening output file\n");
                exit(1);
            }

            dup2(fileno(o), 1);
            int len;
            for(int i = 0;i<num_files; i++){
                len = snprintf(NULL, 0, "/tmp/%d", i)+1;
                params[i+3] = calloc(len, sizeof(char));
                snprintf(params[i+3], len, "/tmp/%d", i);
            }

            params[num_files+3] = NULL;

            execv(params[0], params);
            exit(0);
        }
        wait(0);
    }
    printf("Main Process ends\n");

    return 0;
}