//
// Created by Dima Zhylko on 17/05/2020.
//

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

#define EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(1);}

int queue_capacity;

int current_client_id = -1;

int idle = 0;
pthread_mutex_t idle_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t idle_cond = PTHREAD_COND_INITIALIZER;

int queue_size = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

int is_free = 1;
pthread_mutex_t free_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t free_cond = PTHREAD_COND_INITIALIZER;

void* barber(void* args){

    while(1){

        if(pthread_mutex_lock(&queue_mutex) != 0){
            EXIT("Error: barber: can't lock queue_mutex")
        }

        if(queue_size == 0){
            if(pthread_mutex_lock(&idle_mutex) != 0){
                EXIT("Error: barber: can't lock idle_mutex");
            }
            idle = 1;
            printf("Golibroda: ide spac\n");

            if(pthread_mutex_unlock(&queue_mutex) != 0){
                EXIT("Error: barber: can't unlock queue_mutex")
            }

            while(idle){
                pthread_cond_wait(&idle_cond, &idle_mutex);
            }

            if(pthread_mutex_unlock(&idle_mutex) != 0){
                EXIT("Error: barber: can't unlock idle_mutex");
            }
            printf("Golibroda: czeka %d klientów, golę clienta %d\n", queue_size, current_client_id);
        } else {
            --queue_size;

            printf("Golibroda: czeka %d klientów, golę clienta %d\n", queue_size, current_client_id);

            if(pthread_mutex_unlock(&queue_mutex) != 0){
                EXIT("Error: barber: can't unlock queue_mutex")
            }
        }

        sleep(rand() % 5 + 1);

        if(pthread_mutex_lock(&free_mutex) != 0){
            EXIT("Error: barber: can't lock free_mutex")
        }

        is_free = 1;
        pthread_cond_signal(&free_cond);

        if(pthread_mutex_unlock(&free_mutex) != 0){
            EXIT("Error: barber: can;t unlock free_mutex");
        }
    }
    return NULL;
}

void* client(void* args){

    int id = *((int*) args);

    if(pthread_mutex_lock(&idle_mutex) != 0){
        EXIT("Error: client: can't lock idle_mutex")
    }

    if(idle){
        printf("%d: budze golibrode\n", id);
        idle = 0;
        current_client_id = id;
        pthread_cond_signal(&idle_cond);

        if(pthread_mutex_unlock(&idle_mutex) != 0){
            EXIT("Error: client: can't unlock idle_mutex");
        }

        return NULL;
    }

    if(pthread_mutex_unlock(&idle_mutex) != 0){
        EXIT("Error: client: can't unlock idle_mutex")
    }

    while(1){
        if(pthread_mutex_lock(&queue_mutex) != 0){
            EXIT("Error: client: can't lock queue_mutex")
        }

        if(queue_capacity == queue_size){
            if(pthread_mutex_unlock(&queue_mutex) != 0){
                EXIT("Error: client: can't unlock queue_mutex")
            }

            printf("Zajete, %d\n", id);

            sleep(rand() % 3 + 1);
        } else {
            ++queue_size;

            printf("Poczekalnia, wolne miejsca: %d, %d\n", queue_capacity-queue_size, id);
            if(pthread_mutex_unlock(&queue_mutex) != 0){
                EXIT("Error: client: can't unlock queue_mutex")
            }
            break;
        }
    }

    if(pthread_mutex_lock(&free_mutex) != 0){
        EXIT("Error: client: can't lock free_mutex")
    }

    while(is_free == 0){
        pthread_cond_wait(&free_cond, &free_mutex);
    }

    is_free = 0;

    current_client_id = id;

    if(pthread_mutex_unlock(&free_mutex) != 0){
        EXIT("Error: client: can't unlock free_mutex");
    }

    return NULL;
}

int main(int argc, char** argv){
    if(argc != 3){
        EXIT("Wrong number of arguments")
    }


    queue_capacity = atoi(argv[1]);
    int num_clients = atoi(argv[2]);

    pthread_t barber_id;

    if(pthread_create(&barber_id, NULL, barber, NULL) != 0){
        EXIT("Error: main: can't create barber thread")
    }

    pthread_t clients[num_clients];
    int clients_id[num_clients];

    for(int i = 0;i<num_clients;i++){
        clients_id[i] = i;

        if(pthread_create(&clients[i], NULL, client, (void*) &clients_id[i]) != 0){
            EXIT("Error: main: can't create client thread")
        }

        sleep(1);
    }

    for(int i = 0;i<num_clients;i++){
        if(pthread_join(clients[i], NULL) != 0){
            EXIT("Error: main can't join client thread")
        }
    }

    sleep(3);

    if(pthread_cancel(barber_id) != 0){
        EXIT("Error: main: can't cancel barber thread")
    }

    return 0;
}