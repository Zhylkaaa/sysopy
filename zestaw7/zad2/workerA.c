//
// Created by Dima Zhylko on 27/04/2020.
//

#include "common.h"

int main(){
    shm_id = shm_open(shm_name, O_RDWR, 0660);
    if(shm_id == -1){
        EXIT("workerA: Error creating shared memory. errno: %d", errno)
    }

    shared_memory* memory = mmap(NULL, sizeof(shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if(memory == (void *) -1){
        EXIT("workerA: Error obtaining shared memory. errno: %d", errno)
    }

    sem_t *receive_task = sem_open(sem_names[0], O_RDWR);
    sem_t *block_receive = sem_open(sem_names[3], O_RDWR);
    sem_t *pack = sem_open(sem_names[4], O_RDWR);

    if(receive_task == (void*)-1 || block_receive == (void*)-1 || pack == (void*)-1){
        EXIT("workerA: Error opening semaphores")
    }

    srand(time(NULL));

    while(1){
        sem_wait(receive_task);
        sem_wait(block_receive);

        memory->tasks[memory->put_to] = rand() % MAX_TASK_SIZE;
        ++memory->to_pack;

        printf("(%d %ld) Dodałem liczbę %d: . Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d.\n",\
                    getpid(), time(NULL),\
                    memory->tasks[memory->put_to], \
                    memory->to_pack, memory->to_send);
        fflush(stdout);

        sleep(1);
        memory->put_to = (memory->put_to + 1) % MAX_TASKS;

        sem_post(pack);
        sem_post(block_receive);
    }

    return 0;
}