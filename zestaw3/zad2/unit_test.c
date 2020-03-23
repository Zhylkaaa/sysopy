//
// Created by Dima Zhylko on 23/03/2020.
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
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>

void normalize(int* n, int l, int h){
    if(*n == INT_MIN){
        *n = *n + 1;
    }

    *n = abs(*n);

    *n = (*n)%(h-l+1) + l;
}

void fill_array(int*** M, int n, int m, FILE* random_stream){
    for(int i = 0;i<n;i++){
        (*M)[i] = calloc(m, sizeof(int));
        for(int j = 0;j<m;j++){
            fread(&((*M)[i][j]), sizeof(int), 1, random_stream);
            normalize(&((*M)[i][j]), -100, 100);
        }
    }
}

void write_to_file(int** M, int n, int m, int idx, char* type){
    int len = snprintf(NULL, 0, "m%d.%s", idx, type) + 1;
    char path[len];
    snprintf(path, len, "m%d.%s", idx, type);
    FILE* f = fopen(path, "w");

    for(int i = 0;i<n;i++){
        for(int j = 0;j<m-1;j++){
            fprintf(f, "%d ", M[i][j]);
        }
        if(i == n){
            fprintf(f, "%d", M[i][m-1]);
        } else {
            fprintf(f, "%d\n", M[i][m-1]);
        }
    }
    fflush(f);
    fclose(f);
}

int main() {
    int n = 10;

    FILE *random_stream = fopen("/dev/random", "r");

    int size_min = 3;
    int size_max = 5;

    for (int i = 0; i < n; i++) {
        int m, k, c;
        fread(&m, sizeof(int), 1, random_stream);
        fread(&k, sizeof(int), 1, random_stream);
        fread(&c, sizeof(int), 1, random_stream);

        normalize(&m, size_min, size_max);
        normalize(&k, size_min, size_max);
        normalize(&c, size_min, size_max);

        int **A = calloc(m, sizeof(int *));
        int **B = calloc(k, sizeof(int *));

        fill_array(&A, m, k, random_stream);
        fill_array(&B, k, c, random_stream);

        write_to_file(A, m, k, 0, "a");
        write_to_file(B, k, c, 0, "b");
        int pid;
        if ((pid = fork()) == 0) {
            execl("./main", "./main", "lists", "10", "10", "1", NULL);
            exit(0);
        }

        int** res = calloc(m, sizeof(int*));

        for (int ii = 0;ii<m;ii++){
            res[ii] = calloc(c, sizeof(int));
            for(int jj = 0;jj<c;jj++){
                res[ii][jj] = 0;
                for(int kk = 0;kk<k;kk++){
                    res[ii][jj] += A[ii][kk] * B[kk][jj];
                }
            }
        }

        int status;

        wait(&status);

        FILE* file = fopen("m0.c", "r");

        for (int ii = 0;ii<m;ii++) {
            for (int jj = 0; jj < c; jj++) {
                int t;
                fscanf(file, "%d", &t);
                if(t != res[ii][jj]){
                    printf("Wrong result\n");
                    exit(1);
                }
            }
        }
        fclose(file);

        printf("test #%d passed\n", i);

        for (int ii = 0;ii<m;ii++) {
            free(res[ii]);
            free(A[ii]);
        }
        free(res);
        free(A);
        for (int ii = 0;ii<k;ii++) {
            free(B[ii]);
        }
        free(B);
    }

    return 0;
}
