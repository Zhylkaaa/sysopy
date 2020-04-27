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

#include <mqueue.h>
#include <sys/types.h>

#define MAX_MSG_LEN 256
#define MAX_USERS 20
#define PROJECT_ID 'F'

#define STOP 5
#define DISCONNECT 4
#define LIST 3
#define CONNECT 2
#define INIT 1

#define EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(1);}
#define equals(s1, s2) strcmp(s1, s2) == 0

#define SERVER_Q "/server"

typedef struct {
    pid_t sender_pid;
    char message_str[MAX_MSG_LEN];
} message;

int size = sizeof(message);

mqd_t create_q(const char* path){
    struct mq_attr attr;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = size;

    mqd_t q_id = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0660, &attr);

    if(q_id == -1){
        EXIT("Error creating queue\n errno: %d", errno)
    }

    return q_id;
}

mqd_t open_q(const char* path){
    mqd_t q_id = mq_open(path, O_WRONLY);
    if(q_id == -1){
        EXIT("Error opening queue\n")
    }
    return q_id;
}

#endif //SYSOPY_COMMON_H
