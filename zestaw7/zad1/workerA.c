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
        semop(sem_id, &receive_task, 1);
        semop(sem_id, &block_receive, 1);

        memory->tasks[memory->put_to] = rand() % MAX_TASK_SIZE;
        ++memory->to_pack;

        printf("(%d %d) Dodałem liczbę %d: . Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d.\n",\
                    getpid(), time(NULL),\
                    memory->tasks[memory->put_to], \
                    memory->to_pack, memory->to_send);
        fflush(stdout);

        sleep(1);
        memory->put_to = (memory->put_to + 1) % MAX_TASKS;

        semop(sem_id, &to_pack, 1);
        semop(sem_id, &unblock_receive, 1);
    }

    return 0;
}