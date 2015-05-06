#include "nc_window.h"
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>

uint32_t NC_WIN_Y, NC_WIN_X, NC_WIN_RES;

int ncd_init(nc_data *p, nc_window *parent,
             void *data, size_t size,
             uint32_t pos_y, uint32_t pos_x,
             uint32_t size_y, uint32_t size_x,
             uint32_t flags) {

    p->parent = parent;
    p->data = data;
    if (size > NC_MAX_DATA_SIZE)
        size = NC_MAX_DATA_SIZE;
    p->size = size;
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    p->size_y = size_y;
    p->size_x = size_x;
    p->flags = flags;
    return 0;
}

int ncw_init(nc_window *p,
             uint32_t pos_y, uint32_t pos_x,
             uint32_t size_y, uint32_t size_x,
             uint32_t flags) {

    p->data = malloc(sizeof(nc_data *)*NC_MAX_ARR_LEN);
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
    p->parent = NULL;
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    p->size_y = size_y;
    p->size_x = size_x;
    p->flags = flags;
    p->data_size = 0;

    ncw_resize(p);  // Update size and position from flags
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

    wresize(p->win, GET_SIZE_Y(p), GET_SIZE_X(p));
    mvwin(p->win, GET_POS_Y(p), GET_POS_X(p));

    wclear(stdscr);
    wclear(p->win);

    return 0;
}

int ncw_draw(nc_window *p) {
    int i, k;

    for (i = 0; i < p->data_size; i++) {
        /* Draw each data object */
        nc_data *d = p->data[i];
        if (STRING(d) || TSTRING(d)) {
            int pos_y = GET_POS_Y(d);
            int pos_x = GET_POS_X(d);
            char str[d->size+1];
            char *token;

            strncpy(str, *(char **)d->data, d->size);
            str[d->size] = '\0';

            for (token = strtok(str, "\n"), k = 0; token;
                 token = strtok(NULL, "\n"), k++) {
                if (STRING(d))
                    mvwprintw(p->win, pos_y + k, pos_x, token);
                else {
                    int j;
                    wmove(p->win, pos_y + k, pos_x);
                    for (j = 0; j < strlen(token); j++) {
                        if (token[j] == ' ')
                            wmove(p->win, pos_y + k, pos_x + j + 1);
                        else
                            waddch(p->win, token[j]);
                    }
                }
            }
        }
        else if (BAR(d)) {
        }
    }

    /* Draw border last */
    char *tl, *tr, *bl, *br;
    char *vl, *hl;
    int size_y, size_x;

    getmaxyx(p->win, size_y, size_x);

    if (BORDER_N(p))    // No border
        return 0;

    if (BORDER_THK(p)) {
        tl = "\u250f";
        tr = "\u2513";
        bl = "\u2517";
        br = "\u251b";

        vl = "\u2503";
        hl = "\u2501";
    }
    else if (BORDER_DSH(p)) {
        tl = "\u250c";
        tr = "\u2510";
        bl = "\u2514";
        br = "\u2518";

        vl = "\u254e";
        hl = "\u254c";
    } else {  // default NC_BORDER_THN
        tl = "\u250c";
        tr = "\u2510";
        bl = "\u2514";
        br = "\u2518";

        vl = "\u2502";
        hl = "\u2500";
    }


    mvwprintw(p->win, 0, 0, tl);
    mvwprintw(p->win, size_y-1, 0, bl);
    mvwprintw(p->win, 0, size_x-1, tr);
    mvwprintw(p->win, size_y-1, size_x-1, br);

    for (i = 1; i < size_x-1; i++) {
        mvwprintw(p->win, 0, i, hl);
        mvwprintw(p->win, size_y-1, i, hl);
    }
    for (i = 1; i < size_y-1; i++) {
        mvwprintw(p->win, i, 0, vl);
        mvwprintw(p->win, i, size_x-1, vl);
    }
    return 0;
}

int ncw_add_data(nc_window *p,
                 void *data, size_t size,
                 uint32_t pos_y, uint32_t pos_x,
                 uint32_t size_y, uint32_t size_x,
                 uint32_t flags) {
    if (p->data_size > NC_MAX_ARR_LEN) {
        fprintf(stderr, "Exceeded maximum allowed data objects.\n");
        return -1;
    }

    nc_data *new_data = malloc(sizeof(nc_data));
    if (new_data == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        return -1;
    }
    ncd_init(new_data, p,
             data, size,
             pos_y, pos_x,
             size_y, size_x,
             flags);
    p->data[p->data_size++] = new_data;
    return 0;
}
