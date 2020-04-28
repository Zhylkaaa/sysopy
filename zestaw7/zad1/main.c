//
// Created by Dima Zhylko on 27/04/2020.
//

#include "common.h"

pid_t child[MAX_CHILDR];
int chldrc;

void free_shm_sem(){
    if(sem_id != -1){
        semctl(sem_id, 0, IPC_RMID);
    }

    if(shm_id != -1){
        shmctl(shm_id, IPC_RMID, NULL);
    }
}

void create_shm_sem(){
    /// SEMAPHORE
    char* path = getenv("HOME");
    if(path == NULL){
        EXIT("Can't read $HOME env variable")
    }

    key_t k = ftok(path, SEM_KEY);
    if(k == -1){
        EXIT("Error creating key. errno: %d", errno)
    }

    sem_id = semget(k, 6, IPC_CREAT | IPC_EXCL | IPC_R | IPC_W | IPC_M);
    if(sem_id == -1){
        EXIT("Error creating semaphore. errno: %d", errno)
    }
    ///

    /// SHARED MEMORY
    k = ftok(path, SHM_KEY);
    if(k == -1){
        EXIT("Error creating key. errno: %d", errno)
    }

    shm_id = shmget(k, sizeof(shared_memory), IPC_CREAT | IPC_EXCL | IPC_R | IPC_W | IPC_M);
    if(shm_id == -1){
        EXIT("Error creating shared memory. errno: %d", errno)
    }

    shared_memory* memory = shmat(shm_id, NULL, 0);
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

    shmdt(memory);

    union semun arg1;
    union semun arg2;
    union semun arg3;
    union semun arg4;
    union semun arg5;
    union semun arg6;

    arg1.val = MAX_TASKS;
    arg2.val = 0;
    arg3.val = 0;
    arg4.val = 1;
    arg5.val = 1;
    arg6.val = 1;

    semctl(sem_id, 0, SETVAL, arg1);
    semctl(sem_id, 1, SETVAL, arg2);
    semctl(sem_id, 2, SETVAL, arg3);
    semctl(sem_id, 3, SETVAL, arg4);
    semctl(sem_id, 4, SETVAL, arg5);
    semctl(sem_id, 5, SETVAL, arg6);

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