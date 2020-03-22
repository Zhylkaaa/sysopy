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
#include <errno.h>

void read_row(FILE* f, int** dest, int N){
    for(int i = 0;i<N;i++){
        fscanf(f, "%d", &((*dest)[i]));
    }
}

void read_col(FILE* f, int** dest, int idx, int N, int M){
    int tmp;
    for(int i = 0; i<N;i++){
        for(int j = 0;j<M;j++){
            if(j != idx){
                fscanf(f, "%d", &tmp);
            } else {
                fscanf(f, "%d", &(*dest)[i]);
            }
        }
    }
}

int dot(int* A, int* B, int N){
    int sum = 0;

    for(int i = 0;i<N;i++){
        sum += A[i] * B[i];
    }

    return sum;
}

void print(int* t, int N){
    for(int i = 0;i<N;i++){
        printf("%d ", t[i]);
    }
    printf("\n");
}

int status;
int counter;
int idx;

void check_kill(int com_fd, FILE* com){
    flock(com_fd, LOCK_EX);
    fseek(com, 2*idx*sizeof(char), SEEK_SET);
    fscanf(com, "%d", &status);
    if(status == 0){
        flock(com_fd, LOCK_UN);
        exit(counter);
    }
    flock(com_fd, LOCK_UN);
}

int main(int argc, char** argv){ // "./child", cidx, stride, "communication.tmp", path_to_A, row_A, col_A, path_to_B, row_B, col_B, path_to_C, mode

    if(argc != 12){
        printf("Wrong number of arguments\n");
        exit(0);
    }

    printf("Child process with ID %s\n", argv[1]);

    idx = atoi(argv[1]);
    int stride = atoi(argv[2]);

    int row_A = atoi(argv[5]), col_A = atoi(argv[6]), row_B = atoi(argv[8]), col_B = atoi(argv[9]);
    char* mode = argv[11];

    FILE* f_A = fopen(argv[4], "r+");
    FILE* f_B = fopen(argv[7], "r+");
    FILE* f_C = fopen(argv[10], "w+");

    if(f_A == NULL || f_B == NULL){
        printf("FUCK OFF\n");
        exit(0);
    }

    FILE* com = fopen(argv[3], "r+");
    while(com == NULL){
        com = fopen(argv[3], "r+");
        usleep(1);
    }

    int com_fd = fileno(com);

    int last;
    char** buf = (char**) calloc(row_A, sizeof(char*));
    size_t* sizes = (size_t*) calloc(row_A, sizeof(size_t));
    for(int i = 0;i<row_A;i++){
        buf[i] = NULL;
        sizes[i] = 0;
    }

    /*while(flock(com_fd, LOCK_EX | LOCK_NB) == -1){
        fseek(com, 2*stride*sizeof(char), SEEK_SET);
        printf("%d\n", fscanf(com, "%d", &last));
        printf("%d\n", last);
        usleep(200000);
    }*/

    fseek(com, 2*idx*sizeof(char), SEEK_SET);
    fscanf(com, "%d", &status);

    flock(com_fd, LOCK_UN);
    printf("status: %d\n", status);

    int* B_col = calloc(row_B, sizeof(int));
    int* A_row = calloc(col_A, sizeof(int));
    int* C_col = calloc(row_A, sizeof(int));

    for(int i = idx; i<col_B; i+=stride){
        check_kill(com_fd, com);
        counter += 1;

        fseek(f_B, 0, SEEK_SET);
        fseek(f_A, 0, SEEK_SET);
        read_col(f_B, &B_col, i, row_B, col_B);

        //print(B_col, row_B);

        for(int j = 0; j<row_A; j++){
            read_row(f_A, &A_row, col_A);
            //print(A_row, col_A);
            C_col[j] = dot(A_row, B_col, col_A);
        }

        // TODO: wait for idx of last written column in file to be idx - 1
        // TODO: write result to file and change last written column in file
        fseek(com, 2*stride*sizeof(char), SEEK_SET);
        fscanf(com, "%d", &last);
        while(last != i - 1){
            check_kill(com_fd, com);
            usleep(2);
            fseek(com, 2*stride*sizeof(char), SEEK_SET);
            fscanf(com, "%d", &last);
        }

        if(strcmp(mode, "0") == 0){
            flock(fileno(f_C), LOCK_EX);
            fseek(f_C, 0, SEEK_SET);
            for(int j = 0;j<row_A;j++){
                size_t len = getline(&buf[j], &sizes[j], f_C);
                if(len > 0){
                    buf[j][len-1] = '\0';
                }
            }
            fseek(f_C, 0, SEEK_SET);
            for(int j = 0;j<row_A;j++){
                if(strlen(buf[j]) == 0){
                    fprintf(f_C, "%d\n", C_col[j]);
                } else {
                    fprintf(f_C, "%s %d\n", buf[j], C_col[j]);
                }
            }
            fflush(f_C);
            flock(fileno(f_C), LOCK_UN);

            flock(com_fd, LOCK_EX);
            fseek(com, 2*stride*sizeof(char), SEEK_SET);
            fprintf(com, "%d\n", i);
            fflush(com);
            flock(com_fd, LOCK_UN);
        }
    }

    exit(counter);
}
