//
// Created by Dima Zhylko on 06/04/2020.
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
#include <errno.h>
#include <signal.h>

#define MAX_COMMAND_LEN 1024
#define MAX_ARG_NUM 20
#define MAX_COMMANDS_NUM 30

char** parse_args(char* command){
    char** args = (char**) calloc(MAX_ARG_NUM+2, sizeof(char*));
    int argc = 0;

    char* arg;
    arg = strtok(command, " \n");
    while(arg != NULL){
        args[argc++] = arg;
        arg = strtok(NULL, " \n");
    }
    args[argc] = NULL;

    return args;
}

void exec(char *command, int mode) {
    static int pfd[2]; // prev
    static int cfd[2]; // current

    if (pipe(cfd) != 0) {
        perror("Error during pipe execution\n");
        exit(1);
    }

    if (fork() == 0) {
        char **args = parse_args(command);

        if (mode != 1) { // not the last
            close(cfd[0]);
            if (dup2(cfd[1], STDOUT_FILENO) == -1) {
                perror("Error during dup2 execution");
                exit(1);
            }
        }

        if (mode != -1) { // not the first
            close(pfd[1]);
            if (dup2(pfd[0], STDIN_FILENO) == -1) {
                perror("Error during dup2 execution");
                exit(1);
            }
        }

        if (execvp(args[0], args) == -1) {
            printf("Error during executing command %s\n", args[0]);
            exit(1);
        }
    }

    if (mode == 0) {
        close(pfd[0]);
        close(pfd[1]);
    }
    if (mode != 1) {
        pfd[0] = cfd[0];
        pfd[1] = cfd[1];
    } else {
        close(cfd[0]);
        close(cfd[1]);
    }
}

void interpret(char line[MAX_COMMAND_LEN]) {
    int coms = 0;
    char **commands = (char **) calloc(MAX_COMMANDS_NUM, sizeof(char *));
    char *command;

    command = strtok(line, "|");
    while (command != NULL) {
        commands[coms++] = command;
        command = strtok(NULL, "|");
    }

    int mode = 0;

    for (int i = 0; i < coms; i++) {
        if (i == 0)mode = -1;
        else if (i == coms - 1)mode = 1;
        else mode = 0;
        exec(commands[i], mode);
    }

    wait(NULL);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        exit(1);
    }
    char *file_path = argv[1];
    FILE *f = fopen(file_path, "r");
    if (f == NULL) {
        exit(1);
    }

    char command[MAX_COMMAND_LEN];
    while (fgets(command, MAX_COMMAND_LEN, f) != NULL) {
        interpret(command);
    }

    fclose(f);

    return 0;
}