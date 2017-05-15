nclude <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* Semaphores */
#include <sys/sem.h>

#define LOCK    -1
#define UNLOCK   1

static key_t semKey; // Semaphore
static key_t shmKey; // Shared Memory

struct sembuf semaphore;
int semID;
int semOp(int op) {
    semaphore.sem_op = op;
    semaphore.sem_flg = SEM_UNDO;
    if(semop(semID, &semaphore, 1) == -1) {
        perror("semop error");
        exit(EXIT_FAILURE);
    }
}
int initSemaphor(void) {
    // Check if semaphor exists
    semID = semget(semKey, 0, IPC_PRIVATE);
    if(semID < 0) { 
        // Create a new semaphor
        int semID    = semget(semKey, 1, 0666 |IPC_CREAT | IPC_EXCL);
        if(semID < 0) { // Error ?
            perror("semget fail");
            exit(EXIT_FAILURE);
        }
        // Set initial value to 1
        if(semctl(semID, 0, SETVAL, (int) 1) == -1) {
            perror("semctl fail");
        }
    } 
    
    incSHMVar();
    
}
int main(void) { 
    
    shmKey   = ftok("/tmp/ckey", 'a'); // Shared Memory
    semKey = ftok("/tmp/semKey", 's'); // Semaphore Key 
    /* INIT SEMAPHORE */
    initSemaphor();
     
    /* Loop to create 100 Processes*/
    //return EXIT_SUCCESS;
} 

int incSHMVar(void) {
    puts("tst");
    int shmID   = shmget(2014, 1024, 0666 | IPC_PRIVATE); 
    // Pointer to the segment
    int * data = shmat(shmID, 0,0); 
    printf("Data is: %i", data);
}
