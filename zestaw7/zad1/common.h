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

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEM_KEY 'f'
#define SHM_KEY 'F'
#define MAX_TASKS 2
#define MAX_CHILDR 100000
#define MAX_TASK_SIZE 1000

#define EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(1);}
#define exec(program, args...) execl(program, program, args, NULL)

int shm_id = -1;
int sem_id = -1;

typedef struct shared_memory {
    int to_pack;
    int to_send;
    int take_from;
    int put_to;
    int tasks[MAX_TASKS];
} shared_memory;

struct sembuf block = {.sem_num=3, .sem_op=-1, .sem_flg=0};
struct sembuf unblock = {.sem_num=3, .sem_op=1, .sem_flg=0};

struct sembuf receive_task = {.sem_num=0, .sem_op=-1, .sem_flg=0};
struct sembuf send_task = {.sem_num=0, .sem_op=1, .sem_flg=0};

struct sembuf to_pack = {.sem_num=1, .sem_op=1, .sem_flg=0};
struct sembuf pack = {.sem_num=1, .sem_op=-1, .sem_flg=0};

struct sembuf to_send = {.sem_num=2, .sem_op=1, .sem_flg=0};
struct sembuf send = {.sem_num=2, .sem_op=-1, .sem_flg=0};

void get_shm_sem(){
    char* path = getenv("HOME");
    if(path == NULL){
        EXIT("Can't read $HOME env variable")
    }

    key_t k = ftok(path, SEM_KEY);
    if(k == -1){
        EXIT("Error creating key. errno: %d", errno);
    }

    sem_id = semget(k, 0, IPC_R | IPC_W | IPC_M);
    if(sem_id == -1){
        EXIT("Error creating semaphore. errno: %d", errno);
    }

    k = ftok(path, SHM_KEY);
    if(k == -1){
        EXIT("Error creating key. errno: %d", errno);
    }

    shm_id = shmget(k, 0, IPC_R | IPC_W | IPC_M);
    if(shm_id == -1){
        EXIT("Error creating shared memory. errno: %d", errno);
    }
}

#endif //SYSOPY_COMMON_H
