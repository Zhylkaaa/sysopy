//
// Created by Dima Zhylko on 03/05/2020.
//

#define MAX_VAL 256
#define EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(1);}
#define equals(s1, s2) strcmp(s1, s2) == 0

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
#include <sys/types.h>
#include <pthread.h>

int hist[MAX_VAL];
int m, h, w;
int** image;

typedef struct thread_info {
    int offset;
    int range;
} thread_info;

typedef struct retval {
    int hist[MAX_VAL];
} retval;

void* count_pixels(void* pthread_info){
    thread_info* info = (thread_info*) pthread_info;
    retval* res = (retval*) calloc(1, sizeof(retval));
    int px;
    for(int i = 0;i<h;i++){
        for(int j = 0;j<w;j++){
            px = image[i][j] - info->offset;
            if(px >= 0 && px < info->range)res->hist[image[i][j]]++;
        }
    }
    return (void*) res;
}

void handle_sign(){
    int range = MAX_VAL / m;
    pthread_t pool[m];
    thread_info **args = (thread_info**) calloc(m, sizeof(thread_info*));
    for(int i = 0;i<m;i++){
        args[i] = (thread_info*) calloc(1, sizeof(thread_info));
        args[i]->offset = i*range;
        args[i]->range = range;

        if(pthread_create(&pool[i], NULL, count_pixels, (void*) args[i]) != 0){
            EXIT("Error: can't create thread\n")
        }
    }

    retval* res;
    for(int i = 0;i<m;i++){
        pthread_join(pool[i], (void**)&res);

        for(int j = args[i]->offset;j<args[i]->offset+args[i]->range;j++){
            hist[j] += res->hist[j];
        }
        free(args[i]);
        free(res);
    }

    free(args);
}

void* count_block(void* pthread_info){
    thread_info* info = (thread_info*) pthread_info;
    retval* res = (retval*) calloc(1, sizeof(retval));

    int offset = info->offset;
    int range = info->range;

    int ww = offset + range;
    if(ww > w)ww = w;

    for(int i = 0;i<h;i++){
        for(int j = offset;j<ww;j++){
            res->hist[image[i][j]]++;
        }
    }

    return (void *) res;
}

void handle_block(){
    int range = (w + m - 1) / m;
    pthread_t pool[m];
    thread_info **args = (thread_info**) calloc(m, sizeof(thread_info*));
    for(int i = 0;i<m;i++){
        args[i] = (thread_info*) calloc(1, sizeof(thread_info));
        args[i]->offset = i*range;
        args[i]->range = range;
        if(pthread_create(&pool[i], NULL, count_block, (void*) args[i]) != 0){
            EXIT("Error: can't create thread\n")
        }
    }

    retval* res;
    for(int i = 0;i<m;i++){
        pthread_join(pool[i], (void**)&res);
        free(args[i]);

        for(int j = 0;j<MAX_VAL;j++){
            hist[j] += res->hist[j];
        }
        free(res);
    }
    free(args);
}

void* count_interleaved(void* pthread_info){
    thread_info* info = (thread_info*) pthread_info;
    retval* res = (retval*) calloc(1, sizeof(retval));

    int idx = info->offset;
    int stride = info->range;

    for(int i = 0;i<h;i++){
        for(int j = idx;j<w;j+=stride){
            res->hist[image[i][j]]++;
        }
    }

    return (void *) res;
}

void handle_interleaved(){
    pthread_t pool[m];
    thread_info **args = (thread_info**) calloc(m, sizeof(thread_info*));
    for(int i = 0;i<m;i++){
        args[i] = (thread_info*) calloc(1, sizeof(thread_info));
        args[i]->offset = i; // idx
        args[i]->range = m; // stride
        if(pthread_create(&pool[i], NULL, count_interleaved, (void*) args[i]) != 0){
            EXIT("Error: can't create thread\n")
        }
    }

    retval* res;
    for(int i = 0;i<m;i++){
        pthread_join(pool[i], (void**)&res);
        free(args[i]);

        for(int j = 0;j<MAX_VAL;j++){
            hist[j] += res->hist[j];
        }
        free(res);
    }
    free(args);
}

int main(int argc, char** argv){
    if(argc != 5){
        EXIT("Error: Wrong number of arguments\n")
    }

    m = atoi(argv[1]);
    if(MAX_VAL % m != 0){
        EXIT("Error: number of thread should be divider of 256")
    }

    FILE* in = fopen(argv[3], "r");
    FILE* out = fopen(argv[4], "w+");
    int out_fd = fileno(out);

    char* type[10];
    fscanf(in, "%s", (char*) type);
    if(!(equals((char*) type, "P2"))){
        EXIT("Error: image should be of type PGM\n");
    }

    fscanf(in, "%d %d", &w, &h);
    int tmp;
    fscanf(in, "%d", &tmp);
    image = (int**) calloc(h, sizeof(int*));

    for(int i = 0;i<h;i++){
        image[i] = (int*) calloc(w, sizeof(int));
        for(int j = 0;j<w;j++){
            fscanf(in, "%d", &image[i][j]);
        }
    }

    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);

    if(equals(argv[2], "sign")){
        handle_sign();
    } else if(equals(argv[2], "block")){
        handle_block();
    } else if(equals(argv[2], "interleaved")){
        handle_interleaved();
    } else {
        EXIT("Error: invalid second argument. Allowed values: sign / block / interleaved\n")
    }

    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);

    printf("time: %ld s %ld ns", end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec);
    fflush(stdout);

    if(dup2(out_fd, fileno(stdout)) == -1){
        EXIT("Error: can't redirect output to output file");
    }

    int c = 0;
    for(int i = 0;i<MAX_VAL;i++) {
        printf("Count of %d = %d\n", i, hist[i]);
        c+= hist[i];
    }
    printf("expected sum: %d, obtained sum: %d", h*w, c);

    return 0;
}