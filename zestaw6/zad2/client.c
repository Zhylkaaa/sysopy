//
// Created by Dima Zhylko on 25/04/2020.
//

#include "common.h"

mqd_t main_q;
mqd_t com_q;
pid_t com_pid;
mqd_t my_q;
int my_id;
char path[256];

/*void notify_message() {
    if (com_pid == 0) {
        EXIT("client: Invalid pid")
    }
    kill(com_pid, SIGUSR1);
}*/

void send_disconnect_to_server() {
    message dism;
    dism.sender_pid = getpid();

    if (mq_send(main_q,(char*) &dism, size, DISCONNECT) == -1) {
        EXIT("client: Error can't send disconnect messages to server\n")
    }
    com_pid = 0;
    com_q = 0;
    printf("\n=======server=======\n");
}

void send_disconnect() {
    if (com_pid != 0) {
        message m;
        m.sender_pid = getpid();
        mq_send(com_q,(char*) &m, size, DISCONNECT);
        //notify_message();
        mq_close(com_q);
        send_disconnect_to_server();
    }
}

void send_message(char *m_str) {
    message to_client;
    to_client.sender_pid = getpid();
    strcpy(to_client.message_str, m_str);

    if (mq_send(com_q, (char*) &to_client, size, CONNECT) == -1) {
        EXIT("client: Error can't send messages to client\n");
    }
    //notify_message();
}

void close_and_disconnect() {
    send_disconnect();

    message m;
    m.sender_pid = getpid();
    mq_send(main_q, &m, size, STOP);

    mq_close(main_q);
    mq_unlink(path);
}

void receive_notification() {
    message m;
    uint mtype;

    if (mq_receive(my_q, (char*) &m, size, &mtype) == -1) {
        EXIT("client: Error receiving message from server")
    }

    if (mtype == CONNECT) {
        if(com_pid == 0){
            com_pid = m.sender_pid;
            char com_path[10];
            sprintf((char*) com_path, "/%d", com_pid);
            com_q = open_q(com_path);
            printf("\n========chat========\n");
        } else {
            printf("%s\n", m.message_str);
            fflush(stdout);
        }
        struct sigevent callback;
        callback.sigev_notify = SIGEV_SIGNAL;
        callback.sigev_signo = SIGUSR1;

        mq_notify(my_q, &callback);
    } else if (mtype == DISCONNECT) {
        send_disconnect_to_server();
    }
}

void send_connect(int com_id) {
    if (com_id == my_id) {
        printf("Can't connect to yourself\n");
        return;
    }

    message m;
    m.sender_pid = getpid();
    sprintf(m.message_str, "%d", com_id);

    if (mq_send(main_q, (char*)&m, size, CONNECT) == -1) {
        EXIT("client: Error can't send messages to server\n");
    }

    message from_server;

    if (mq_receive(my_q, (char*)&from_server, size, NULL) == -1) {
        EXIT("client: Error receiving message from server")
    }

    sscanf(from_server.message_str, "%d", &com_pid);

    if (com_pid == 0) {
        EXIT("client: Error connecting to client id %d", com_id)
    }

    char* com_path[256];
    sprintf((char*)com_path, "/%d", com_pid);

    com_q = open_q(com_path);

    message to_client;
    to_client.sender_pid = getpid();
    sprintf(to_client.message_str, "%d", getpid());

    if (mq_send(com_q, (char*)&to_client, size, CONNECT) == -1) {
        EXIT("client: Error can't send messages to client\n");
    }

    printf("\n========chat========\n");
    struct sigevent callback;
    callback.sigev_notify = SIGEV_SIGNAL;
    callback.sigev_signo = SIGUSR1;

    mq_notify(my_q, &callback);
    // notify_message();
}

void send_list() {
    message to_server;
    to_server.sender_pid = getpid();

    if (mq_send(main_q, &to_server, size, LIST) == -1) {
        EXIT("client: Error can't send messages to server\n");
    }
}

void handle_sigint() { exit(1); }

void send_init(){
    message to_client;
    to_client.sender_pid = getpid();

    if (mq_send(main_q, (char*) &to_client, size, INIT) == -1) {
        EXIT("client: Error can't send messages to server\n");
    }
    message from_server;

    if (mq_receive(my_q, &from_server, size, NULL) == -1) {
        EXIT("client: Error receiving message from server")
    }

    sscanf(from_server.message_str, "%d", &my_id);
    printf("client: Your ID is %d\n", my_id);
    fflush(stdout);
}

int main() {
    if (atexit(close_and_disconnect) != 0) {
        EXIT("client: Error calling atexit\n")
    }

    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        EXIT("client: Error of SIGINT handler setup\n")
    }

    if (signal(SIGUSR1, receive_notification) == SIG_ERR) {
        EXIT("client: Error of SIGUSR1 handler setup\n")
    }

    sprintf((char*) path, "/%d", getpid());

    main_q = open_q(SERVER_Q);

    my_q = create_q(path);

    send_init();

    char *in[MAX_MSG_LEN];

    struct sigevent callback;
    callback.sigev_notify = SIGEV_SIGNAL;
    callback.sigev_signo = SIGUSR1;

    mq_notify(my_q, &callback);

    while (1) {
        gets((char *) in);

        if (equals((const char *) in, "DISCONNECT")) {
            send_disconnect();
            continue;
        }

        if (com_q != 0) {
            send_message((char *) in);
            continue;
        }

        if (equals((const char *) in, "LIST")) {
            send_list();
        } else if (equals((const char *) in, "STOP")) {
            exit(0);
        } else {
            char command[MAX_MSG_LEN];
            int com_id;
            sscanf((const char *) in, "%s %d", command, &com_id);

            if (equals(command, "CONNECT")) {
                send_connect(com_id);
            } else {
                printf("Unknown command %s\n", command);
                fflush(stdout);
            }
        }
    }

    return 0;
}
