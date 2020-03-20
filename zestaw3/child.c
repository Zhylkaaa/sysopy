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

int main(int argc, char** argv){ // "./child", cidx, stride, argv[3], path_to_A, row_A, col_A, path_to_B, row_B, col_B

    if(argc != 10){
        printf("Wrong number of arguments\n");
        exit(0);
    }

    printf("Child process with ID %s\n", argv[1]);

    int idx = atoi(argv[1]);
    int stride = atoi(argv[2]);

    // TODO: implement time_out ?
    useconds_t time_out = atoi(argv[3]) * 1000;
    int row_A = atoi(argv[5]), col_A = atoi(argv[6]), row_B = atoi(argv[8]), col_B = atoi(argv[9]);

    FILE* f_A = fopen(argv[4], "r+");
    FILE* f_B = fopen(argv[7], "r+");

    if(f_A == NULL || f_B == NULL){
        printf("FUCK OFF\n");
        exit(0);
    }

    int* B_col = calloc(row_B, sizeof(int));
    int* A_row = calloc(col_A, sizeof(int));
    int* C_col = calloc(row_A, sizeof(int));

    int counter = 0;

    for(int i = idx; i<col_B; i+=stride){
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
        // TODO : write to file (how the fuck I am supposed to do this?)
        for(int i = 0;i<row_A;i++){
            printf("%d ", C_col[i]);
        }
        printf("\n");

    }
    exit(counter);
}
