//
// Created by Dima Zhylko on 27/04/2020.
//

#include "common.h"

pid_t child[MAX_CHILDR];
int chldrc;

void free_shm_sem(){
    for(int i = 0;i<NUM_SEMS;i++){
        if(sem_ids[i] != SEM_FAILED){
            sem_close(sem_ids[i]);
            sem_unlink(sem_names[i]);
        }
    }

    if(shm_id != -1){
        shm_unlink(shm_name);
    }
}

void create_shm_sem(){
    /// SEMAPHORE
    sem_ids[0] = sem_open(sem_names[0], O_CREAT | O_EXCL, 0660, MAX_TASKS);
    for(int i = 1; i < 4; i++){
        sem_ids[i] = sem_open(sem_names[i], O_CREAT | O_EXCL, 0660, 1);
    }
    for(int i = 4;i<NUM_SEMS;i++){
        sem_ids[i] = sem_open(sem_names[i], O_CREAT | O_EXCL, 0660, 0);
    }

    for(int i = 0; i<NUM_SEMS;i++){
        if(sem_ids[i] == SEM_FAILED){
            EXIT("Error creating semaphore. errno: %d", errno)
        }
    }
    ///

    /// SHARED MEMORY

    shm_id = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0660);

    if(shm_id == -1){
        EXIT("Error creating shared memory. errno: %d", errno)
    }

    if(ftruncate(shm_id, sizeof(shared_memory)) == -1){
        EXIT("Error truncating shared memory. errno: %d", errno);
    }

    shared_memory* memory = mmap(NULL, sizeof(shared_memory), PROT_WRITE, MAP_SHARED, shm_id, 0);
    if(memory == (void *) -1){
        EXIT("Error obtaining shared memory. errno: %d", errno)
    }

    ///

    /// SET VALUES
    memory->put_to = 0;
    memory->pack_from = 0;
    memory->send_from = 0;
    memory->to_pack = 0;
    memory->to_send = 0;

    if(munmap(memory, sizeof(shared_memory)) == -1){
        EXIT("Error unmapping shared memory. errno: %d", errno)
    }
    ///
}

void handle_sigint(){
    for(int i = 0;i<chldrc;i++){
        kill(child[i], 15);
    }

    exit(0);
}

int main(int argc, char** argv){
    if(argc != 4){
        EXIT("Wrong number of arguments")
    }

    if(atexit(free_shm_sem) != 0){
        EXIT("Error calling atexit\n")
    }

    if(signal(SIGINT, handle_sigint) == SIG_ERR){
        EXIT("server: Error of SIGINT handler setup\n")
    }

    create_shm_sem();

    int wA = atoi(argv[1]);
    int wB = atoi(argv[2]);
    int wC = atoi(argv[3]);

    for(int i = 0;i<wA;i++){
        if((child[chldrc++] = fork()) == 0)exec("./workerA", NULL);
    }
    for(int i = 0;i<wB;i++){
        if((child[chldrc++] = fork()) == 0)exec("./workerB", NULL);
    }
    for(int i = 0;i<wC;i++){
        if((child[chldrc++] = fork()) == 0)exec("./workerC", NULL);
    }


    for(int i = 0;i<chldrc;i++){
        waitpid(child[i], NULL, 0);
    }

    return 0;
}