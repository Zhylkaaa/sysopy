//
// Created by Dima Zhylko on 27/04/2020.
//

#include "common.h"

int main(){

    get_shm_sem();
    shared_memory* memory = shmat(shm_id, NULL, 0);
    if(memory == (void *) -1){
        EXIT("Error obtaining shared memory. errno: %d", errno);
    }

    srand(time(NULL));

    while(1){
        semop(sem_id, &send, 1);
        semop(sem_id, &block_send, 1);

        memory->tasks[memory->send_from] *= 3;
        --memory->to_send;

        printf("(%d %d) Wysłałem zamówienie o wielkości %d: . Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d.\n",\
                    getpid(), time(NULL),\
                    memory->tasks[memory->send_from], \
                    memory->to_pack, memory->to_send);

        memory->send_from = (memory->send_from + 1) % MAX_TASKS;

        fflush(stdout);
        sleep(1);
        semop(sem_id, &send_task, 1);
        semop(sem_id, &unblock_send, 1);
    }

    return 0;
}