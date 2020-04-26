//
// Created by Dima Zhylko on 25/04/2020.
//

#include "common.h"

pid_t users[MAX_USERS];
int usr_q[MAX_USERS];
int usr_connected[MAX_USERS];
int user_count;
int main_q;

void handle_sigint(){exit(0);}
void close_queue(){
    for(int i = 0;i<user_count;i++){
        kill(users[i], SIGINT);
    }
    msgctl(main_q, IPC_RMID, NULL);
}

void init_client(message *m){
    int i = 0;
    for(;i<user_count;i++){
        if(users[i] == 0)break;
    }

    users[i] = m->sender_pid;
    usr_q[i] = atoi(m->message_str);

    message to_client;
    to_client.mtype = INIT;
    to_client.sender_pid = getpid();
    sprintf(to_client.message_str, "%d", i);

    if(user_count == i)user_count++;
    if(msgsnd(usr_q[i], &to_client, size, 0) == -1){
        EXIT("server: Error can't send messages\n");
    }
}

int find_usr_by_pid(pid_t p){
    for(int i = 0;i<user_count;i++){
        if(users[i] == p){
            return i;
        }
    }

    return 0;
}

void connect_clients(message *m){
    int client_id = atoi(m->message_str);

    message to_client;
    to_client.mtype = CONNECT;
    to_client.sender_pid = getpid();
    int idx = find_usr_by_pid(m->sender_pid);

    if(client_id >= user_count){
        sprintf(to_client.message_str, "%d %d", -1, users[client_id]);
    } else {
        sprintf(to_client.message_str, "%d %d", usr_q[client_id], users[client_id]);
        usr_connected[idx] = 1;
        usr_connected[client_id] = 1;
    }

    if(msgsnd(usr_q[idx], &to_client, size, 0) == -1){
        EXIT("server: Error can't send messages\n");
    }
}

void list_clients(message *m){
    printf("======USERS======\n");
    printf("client id - is connected\n");
    for(int i = 0;i<user_count;i++){
        if(users[i] == 0)continue;
        printf(" %d      -    %d\n", i, usr_connected[i]);
    }
    printf("=================\n");
}

void disconnect_users(message *m){
    int idx = find_usr_by_pid(m->sender_pid);
    usr_connected[idx] = 0;
}

void stop_user(message *m){
    int idx = find_usr_by_pid(m->sender_pid);
    users[idx] = 0;
    usr_connected[idx] = 0;
    usr_q[idx] = 0;
}

void process_message(message *m){
    switch (m->mtype){
        case INIT:
            init_client(m);
            break;
        case CONNECT:
            connect_clients(m);
            break;
        case LIST:
            list_clients(m);
            break;
        case DISCONNECT:
            disconnect_users(m);
            break;
        case STOP:
            stop_user(m);
            break;
        default:
            printf("server: Error parsing message in message queue\n");
    }
}

int main(){
    if(atexit(close_queue) != 0){
        EXIT("server: Error calling atexit\n")
    }

    if(signal(SIGINT, handle_sigint) == SIG_ERR){
        EXIT("server: Error of SIGINT handler setup\n")
    }

    char* path = getenv("HOME");
    if(path == NULL){
        EXIT("server: Can't read $HOME env variable")
    }

    main_q = create_q(path, PROJECT_ID);

    message m;
    while(1){
        if(msgrcv(main_q, &m, size, -INIT-1, 0) == -1){
            EXIT("server: Error receiving message")
        }

        process_message(&m);
    }

    return 0;
}