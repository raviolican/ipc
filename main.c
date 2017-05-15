ange this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: sappy
 * 
 * Program creates a shared mem. segment and stores 0 in it. It reads
 * a fifo in an endless loop and prints contents to console..
 * Created on May 6, 2017, 2:43 PM
 */


#include <stdio.h>
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

/* 
 * 
 */
int main(int argc, char** argv) {
    
    /* Create Key, should be a unique key */
    key_t kKey = ftok("/tmp/ckey", 'a');
    
    /* Shared Memory */
    // Connect to the segment
    int shmID = shmget(2014, 1024, 0666 | IPC_CREAT); 
    // Pointer to the segment
    int * data = shmat(shmID, 0, 0);  
    data = 3; // Init segment with Zero
    printf("Data is: %i\n", data);
    printf("ID is: %i", shmID);
    
    /* FIFO */
    // Check if File exists
    if(!access("/tmp/RESULT_FIFO", F_OK)) 
        if(unlink("/tmp/RESULT_FIFO") == -1) { // Delete File
            perror("Error unlinking");
            exit(EXIT_FAILURE);
        }
    // Create FIFO
    if(mkfifo("/tmp/RESULT_FIFO", S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("Error while creating FIFO");
        exit(EXIT_FAILURE);
    }
    
    /* LOGGING */
    // Enable Logging
    pid_t pLog = fork();
    if(pLog > 0) {
        if(printFIFOVal("/tmp/RESULT_FIFO")) {
            if (shmdt(data) == -1) {
                perror("Error while detaching from segment");
                exit(EXIT_FAILURE);
            }
        }
    } else if(pLog < 0) {
        perror("Error while forking");
        exit(EXIT_FAILURE);
    } else {
         //  
    }
          
    //exit(EXIT_SUCCESS);
}

void printFIFOVal(char path[]) {
    int fd_fifo = -1, maxfd = -1;
    /* Open FIFO */
    fd_fifo = open(path,0666);
    maxfd = (fd_fifo > maxfd) ? fd_fifo : maxfd;
    // Compile settings
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd_fifo, &readfds);
    
    /* Create read buffer with 256 B*/
    char cBuffer[256];
    for(;;) {
        // When file is ready 
        int ready = select(maxfd+1, &readfds, NULL, NULL, NULL);
        if(ready) {
            // Check if file descriptor is set
            if(FD_ISSET(fd_fifo, &readfds)) {
                if(cBuffer == "die"){
                    return 1;
                } else {
                    read(fd_fifo, cBuffer, sizeof(cBuffer));
                } 
            }
            printf("%s", cBuffer); // print contents to console
        }
    }
}

