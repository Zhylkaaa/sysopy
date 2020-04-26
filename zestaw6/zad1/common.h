//
// Created by Dima Zhylko on 25/04/2020.
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

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define MAX_MSG_LEN 256
#define MAX_USERS 20
#define PROJECT_ID 'F'

#define STOP 1
#define DISCONNECT 2
#define LIST 3
#define CONNECT 4
#define INIT 5

#define EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(1);}
#define equals(s1, s2) strcmp(s1, s2) == 0

typedef struct {
    long mtype;
    pid_t sender_pid;
    char message_str[MAX_MSG_LEN];
} message;

int size = sizeof(message) - sizeof(long);

int create_q(const char* path, char proj_id){
    key_t p_key = ftok(path, proj_id);
    if(p_key == -1){
        EXIT("Error obtaining queue key\n")
    }

    int q_id = msgget(p_key, IPC_CREAT | IPC_EXCL | 0666);

    if(q_id == -1){
        EXIT("Error creating queue\n errno: %d", errno)
    }

    return q_id;
}

#endif //SYSOPY_COMMON_H
