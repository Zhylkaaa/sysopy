//
// Created by Dima Zhylko on 25/04/2020.
//

#include "common.h"

int main_q;
int com_q;
pid_t com_pid;
int my_q;
int my_id;

void notify_message() {
    if (com_pid == 0) {
        EXIT("client: Invalid pid")
    }
    kill(com_pid, SIGUSR1);
}

void send_disconnect_to_server() {
    message dism;
    dism.mtype = DISCONNECT;
    dism.sender_pid = getpid();

    if (msgsnd(main_q, &dism, size, 0) == -1) {
        EXIT("client: Error can't send disconnect messages to server\n")
    }
    com_pid = 0;
    com_q = 0;
    printf("\n=======server=======\n");
}

void send_disconnect() {
    if (com_q != 0) {
        message m;
        m.mtype = DISCONNECT;
        m.sender_pid = getpid();
        msgsnd(com_q, &m, size, 0);
        notify_message();
        send_disconnect_to_server();
    }
}

void send_message(char *m_str) {
    message to_client;
    to_client.mtype = CONNECT;
    to_client.sender_pid = getpid();
    strcpy(to_client.message_str, m_str);

    if (msgsnd(com_q, &to_client, size, 0) == -1) {
        EXIT("client: Error can't send messages to client\n");
    }
    notify_message();
}

void close_and_disconnect() {
    send_disconnect();

    message m;
    m.mtype = STOP;
    m.sender_pid = getpid();
    msgsnd(main_q, &m, size, 0);

    msgctl(my_q, IPC_RMID, NULL);
}

void send_connect(int com_id) {
    if (com_id == my_id) {
        printf("Can't connect to yourself\n");
        return;
    }

    message m;
    m.mtype = CONNECT;
    m.sender_pid = getpid();
    sprintf(m.message_str, "%d", com_id);

    if (msgsnd(main_q, &m, size, 0) == -1) {
        EXIT("client: Error can't send messages to server\n");
    }

    message from_server;

    if (msgrcv(my_q, &from_server, size, CONNECT, 0) == -1) {
        EXIT("client: Error receiving message from server")
    }

    sscanf(from_server.message_str, "%d %d", &com_q, &com_pid);

    if (com_q == -1 || com_pid == 0) {
        EXIT("client: Error connecting to client id %d", com_id)
    }

    message to_client;
    to_client.mtype = CONNECT;
    to_client.sender_pid = getpid();
    sprintf(to_client.message_str, "%d", my_q);

    if (msgsnd(com_q, &to_client, size, 0) == -1) {
        EXIT("client: Error can't send messages to client\n");
    }

    printf("\n========chat========\n");
    notify_message();
}

void send_list() {
    message to_server;
    to_server.mtype = LIST;
    to_server.sender_pid = getpid();

    if (msgsnd(main_q, &to_server, size, 0) == -1) {
        EXIT("client: Error can't send messages to server\n");
    }
}

void receive_notification() {
    message m;
    if (msgrcv(my_q, &m, size, -INIT - 1, 0) == -1) {
        EXIT("client: Error receiving message from server")
    }

    if (m.mtype == CONNECT) {
        if(com_q == 0){
            sscanf(m.message_str, "%d", &com_q);
            com_pid = m.sender_pid;
            printf("\n========chat========\n");
            return;
        }
        printf("%s\n", m.message_str);
        fflush(stdout);
    } else if (m.mtype == DISCONNECT) {
        send_disconnect_to_server();
    }
}

void handle_sigint() { exit(1); }

void send_init(){
    message to_client;
    to_client.mtype = INIT;
    to_client.sender_pid = getpid();
    sprintf(to_client.message_str, "%d", my_q);

    if (msgsnd(main_q, &to_client, size, 0) == -1) {
        EXIT("client: Error can't send messages to server\n");
    }

    message from_server;

    if (msgrcv(my_q, &from_server, size, INIT, 0) == -1) {
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

    char *path = getenv("HOME");
    if (path == NULL) {
        EXIT("server: Can't read $HOME env variable")
    }

    key_t k = ftok(path, PROJECT_ID);
    if (k == -1) {
        EXIT("Error obtaining queue key\n")
    }

    main_q = msgget(k, 0);
    if (main_q == -1) {
        EXIT("client: Error obtaining main message queue ID\n")
    }

    my_q = create_q(path, (char) (getpid() % 'a'));

    send_init();

    char *in[MAX_MSG_LEN];

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