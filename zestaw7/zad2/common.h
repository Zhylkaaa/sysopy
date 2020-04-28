//
// Created by Dima Zhylko on 27/04/2020.
//

#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <fcntl.h>

#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

#define MAX_TASKS 1
#define MAX_CHILDR 100000
#define MAX_TASK_SIZE 1000
#define NUM_SEMS 6

#define EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(1);}
#define exec(program, args...) execl(program, program, args, NULL)

int shm_id = -1;
sem_t* sem_ids[NUM_SEMS];

typedef struct shared_memory {
    int to_pack;
    int to_send;
    int pack_from;
    int send_from;
    int put_to;
    int tasks[MAX_TASKS];
} shared_memory;

const char shm_name[] = "/shared_memory";

const char* sem_names[NUM_SEMS] = {
        "/receive_task",
        "/block_pack",
        "/block_send",
        "/block_receive",
        "/pack",
        "/send"
};

#endif //SYSOPY_COMMON_H
