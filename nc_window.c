#include "nc_window.h"
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>

uint32_t NC_WIN_Y, NC_WIN_X, NC_WIN_RES;

int ncw_init(nc_window *p,
        int pos_y, int pos_x,
        uint32_t size_y, uint32_t size_x,
        uint16_t flags) {

    p->data = malloc(sizeof(nc_data *)*NCW_MAX_ARR_LEN);
    if (p->data == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        return -1;
    }

    p->win = newwin(size_y, size_x, pos_y, pos_x);
    if (p->win == NULL) {
        fprintf(stderr, "Could not create window.\n");
        free(p->data);
        return -1;
    }
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    p->size_y = size_y;
    p->size_x = size_x;
    p->flags = flags;
    p->data_size = 0;
    ncw_resize(p);

    return 0;
}
int ncw_update() {
    int new_y, new_x;
    getmaxyx(stdscr, new_y, new_x);
    if (new_y != NC_WIN_Y || new_x != NC_WIN_X) {
        NC_WIN_Y = new_y;
        NC_WIN_X = new_x;
        NC_WIN_RES |= 1;

        return 1;
    }
    return 0;
}
int ncw_resize(nc_window *p) {
    int i;

    p->asize_y = GET_SIZE_Y(p, NC_WIN_Y);
    p->asize_x = GET_SIZE_X(p, NC_WIN_X);

    p->apos_y = GET_POS_Y(p, NC_WIN_Y);
    p->apos_x = GET_POS_X(p, NC_WIN_X);

    /* Resize data objects */
    for (i = 0; i < p->data_size; i++) {
        nc_data *d = p->data[i];
        d->asize_y = GET_SIZE_Y(d, p->asize_y);
        d->asize_x = GET_SIZE_X(d, p->asize_x);

        d->apos_y = GET_POS_Y(d, p->asize_y);
        d->apos_x = GET_POS_X(d, p->asize_x);
    }

    return 0;
}
int ncw_draw(nc_window *p) {
    int i;

    /* Draw data objects */
    for (i = 0; i < p->data_size; i++) {
        ncd_draw(p->data[i], p->win);
    }

    /* Draw border */
    return 0;
}
int ncd_init(nc_data *p,
        const char *cmd, uint32_t period,
        int pos_y, int pos_x,
        uint32_t size_y, uint32_t size_x,
        uint8_t flags) {

    nc_command *ncc = malloc(sizeof(nc_command));
    if (ncc == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        return -1;
    }
    if (ncc_init(ncc, cmd, period)) {
        free(ncc);
        return -1;
    }
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    p->size_y = size_y;
    p->size_x = size_x;
    p->flags = flags;

    return 0;
}
int ncd_draw(nc_data *p, WINDOW *win) {
    char buf[NCC_BUFFER_SIZE];
    ncc_get(p->cmd, buf);
    p->draw(p->ncd_child, win, buf);
    return 0;
}
