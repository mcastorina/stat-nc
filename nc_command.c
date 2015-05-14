#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "nc_command.h"

uint8_t ncc_end;

void ncc_run(void *);

int ncc_init(nc_command *p, const char *cmd, uint32_t period) {
    p->lock = malloc(sizeof(sem_t));
    if (p->lock == NULL)
        goto ncc_init_err0;

    p->buffer = malloc(sizeof(char) * NCC_BUFFER_SIZE);
    if (p->buffer == NULL)
        goto ncc_init_err1;

    if (sem_init(p->lock, 0, 1))
        goto ncc_init_err2;

    p->cmd = cmd;
    p->period = period;
    ncc_end = 0;

    if (pthread_create(&p->thread, NULL, (void *)ncc_run, (void *)p)) {
        fprintf(stderr, "Error creating thread.\n");
        p->cmd = NULL;
        goto ncc_init_err2;
    }

    return 0;

ncc_init_err2:
    free(p->buffer);
ncc_init_err1:
    free(p->lock);
ncc_init_err0:
    fprintf(stderr, "Could not allocate memory.\n");
    return -1;
}
int ncc_destroy(nc_command *p) {
    /* Notify thread to stop */
    ncc_end = 1;

    /* Wait for thread */
    pthread_join(p->thread, NULL);

    /* Free resources */
    free(p->lock);
    p->lock = NULL;
    free(p->buffer);
    p->buffer = NULL;

    p->cmd = NULL;
    return 0;
}
int ncc_get(nc_command *p, char *dest) {
    /* Acquire lock */
    if (sem_wait(p->lock)) {
        fprintf(stderr, "Error getting lock.\n");
        return -1;
    }

    /* Copy data to dest */
    strncpy(dest, p->buffer, NCC_BUFFER_SIZE);

    /* Release lock */
    sem_post(p->lock);
    return 0;
}

void ncc_run(void *ptr) {
    nc_command *p = (nc_command *)ptr;

    /* Acquire lock */

    /* Run command via popen */

    /* Release lock */
}

