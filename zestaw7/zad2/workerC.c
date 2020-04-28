//
// Created by Dima Zhylko on 27/04/2020.
//

#include "common.h"

int main(){

    shm_id = shm_open(shm_name, O_RDWR, 0660);
    if(shm_id == -1){
        EXIT("workerB: Error creating shared memory. errno: %d", errno)
    }

    shared_memory* memory = mmap(NULL, sizeof(shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if(memory == (void *) -1){
        EXIT("workerB: Error obtaining shared memory. errno: %d", errno)
    }

    sem_t *send = sem_open(sem_names[5], O_RDWR);
    sem_t *block_send = sem_open(sem_names[2], O_RDWR);
    sem_t *receive_task = sem_open(sem_names[0], O_RDWR);

    if(send == (void*)-1 || block_send == (void*)-1 || receive_task == (void*)-1){
        EXIT("workerB: Error opening semaphores")
    }

    srand(time(NULL));

    while(1){
        sem_wait(send);
        sem_wait(block_send);

        memory->tasks[memory->send_from] *= 3;
        --memory->to_send;

        printf("(%d %d) Wysłałem zamówienie o wielkości %d: . Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d.\n",\
                    getpid(), time(NULL),\
                    memory->tasks[memory->send_from], \
                    memory->to_pack, memory->to_send);

        memory->send_from = (memory->send_from + 1) % MAX_TASKS;

        fflush(stdout);
        sleep(1);
        sem_post(receive_task);
        sem_post(block_send);
    }

    return 0;
}