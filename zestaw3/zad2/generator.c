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

FILE* random_stream;

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
    fclose(f);
}

int main(int argc, char** argv){

    if(argc != 4){
        printf("Wrong number of arguments\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    int size_min = atoi(argv[2]);
    int size_max = atoi(argv[3]);

    random_stream = fopen("/dev/random", "r");
    if(random_stream == NULL){
        printf("Can't open /dev/random\n");
        exit(1);
    }

    for(int i = 0;i<n;i++){
        int m, k, c;
        fread(&m, sizeof(int), 1, random_stream);
        fread(&k, sizeof(int), 1, random_stream);
        fread(&c, sizeof(int), 1, random_stream);

        normalize(&m, size_min, size_max);
        normalize(&k, size_min, size_max);
        normalize(&c, size_min, size_max);

        int** A = calloc(m, sizeof(int*));
        int** B = calloc(k, sizeof(int*));

        fill_array(&A, m, k, random_stream);
        fill_array(&B, k, c, random_stream);

        write_to_file(A, m, k, i, "a");
        write_to_file(B, k, c, i, "b");
    }

    fclose(random_stream);

    return 0;
}