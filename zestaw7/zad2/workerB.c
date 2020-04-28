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
    sem_t *block_pack = sem_open(sem_names[1], O_RDWR);
    sem_t *pack = sem_open(sem_names[4], O_RDWR);

    if(send == (void*)-1 || block_pack == (void*)-1 || pack == (void*)-1){
        EXIT("workerB: Error opening semaphores")
    }

    srand(time(NULL));

    while(1){
        sem_wait(pack);
        sem_wait(block_pack);

        memory->tasks[memory->pack_from] *= 2;
        --memory->to_pack;
        ++memory->to_send;
        printf("(%d %d) Przygotowałem zamówienie o wielkości %d: . Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d.\n",\
                    getpid(), time(NULL),\
                    memory->tasks[memory->pack_from], \
                    memory->to_pack, memory->to_send);

        memory->pack_from = (memory->pack_from + 1) % MAX_TASKS;

        fflush(stdout);
        sleep(1);
        sem_post(send);
        sem_post(block_pack);
    }

    return 0;
}