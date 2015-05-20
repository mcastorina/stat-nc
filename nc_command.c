#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "nc_command.h"

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
    p->ncc_end = !period;

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
    p->ncc_end = 1;

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
    if (sem_trywait(p->lock))
        return -1;

    /* Copy data to dest */
    strncpy(dest, p->buffer, NCC_BUFFER_SIZE);

    /* Release lock */
    sem_post(p->lock);
    return 0;
}

void ncc_run(void *ptr) {
    char buf[NCC_BUFFER_SIZE], *dbl_buf, *tmp;
    nc_command *p = (nc_command *)ptr;
    FILE *fp;

    dbl_buf = buf;
    do {
        /* Run command */
        fp = popen(p->cmd, "r");

        if (fp == NULL) {
            fprintf(stderr, "Failed to run command\n");
            continue;
        }

        /* Copy output to second buffer */
        fread(dbl_buf, NCC_BUFFER_SIZE, 1, fp);

        /* Acquire lock */
        if (sem_wait(p->lock)) {
            fprintf(stderr, "Error getting lock.\n");
            continue;
        }

        /* Swap double buffer */
        tmp = p->buffer;
        p->buffer = dbl_buf;
        dbl_buf = tmp;

        /* Release lock */
        sem_post(p->lock);

        /* Close pipe */
        pclose(fp);
        fp = NULL;

        /* Sleep */
        usleep(p->period * 1000);
    }
    while (!p->ncc_end);

    /* Make sure p->buffer points to valid memory */
    if (p->buffer == buf) {
        /* Acquire lock */
        while (sem_wait(p->lock));

        /* Copy data to buffer in heap */
        strncpy(dbl_buf, buf, NCC_BUFFER_SIZE);
        p->buffer = dbl_buf;

        /* Release lock */
        sem_post(p->lock);
    }
}

