#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef __NC_COMMAND_H
#define __NC_COMMAND_H

#define NCC_BUFFER_SIZE     (4096)


typedef struct nc_command {
    const char *cmd;            /* Command to run */
    uint32_t period;            /* Period in ms */
    char *buffer;               /* Buffer to store output */
    pthread_t thread;           /* Thread identifier */
    sem_t *lock;                /* Synchronization for buffer */
} nc_command;

/*=Initializes nc_command object============================================*/
/* nc_command *p            nc_command pointer                              */
/* const char *cmd          Command to run                                  */
/* uint32_t period          Period to run the command in milliseconds       */
/*                                                                          */
/* Initializes a nc_command object with command cmd. This function will     */
/* create a thread that will run the ncc_run command and then return        */
/* 0 on success and -1 on failure. It does not wait for the thread to       */
/* complete before returning.                                               */
/*==========================================================================*/
int ncc_init(nc_command *p, const char *cmd, uint32_t period);

/*=Destroys nc_command object===============================================*/
/* nc_command *p            nc_command pointer                              */
/*                                                                          */
/* Waits for thread to finish, frees allocation, and NULLs pointers.        */
/*==========================================================================*/
int ncc_destroy(nc_command *p);

/*=Copies string from buffer================================================*/
/* nc_command *p            nc_command pointer                              */
/* char *dest               Buffer to put output                            */
/*                                                                          */
/* Copies from the nc_command buffer to the destination. It first acquires  */
/* the lock to ensure the thread isn't writing data as we read it. Note     */
/* that this function can block. Returns 0 on success and -1 on failure.    */
/*==========================================================================*/
int ncc_get(nc_command *p, char *dest);

#endif
