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
        semop(sem_id, &pack, 1);
        semop(sem_id, &block_pack, 1);

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
        semop(sem_id, &to_send, 1);
        semop(sem_id, &unblock_pack, 1);
    }

    return 0;
}