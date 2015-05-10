#include "nc_window.h"
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>

uint32_t NC_WIN_Y, NC_WIN_X, NC_WIN_RES;

int ncd_init(nc_data *p, nc_window *parent,
             void *data, size_t size,
             int pos_y, int pos_x,
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
             int pos_y, int pos_x,
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
    int i, j, k;
    int pos_y, pos_x;

    for (i = 0; i < p->data_size; i++) {
        /* Draw each data object */
        nc_data *d = p->data[i];

        pos_y = BOUND((int)GET_POS_Y(d),
                (int)HAS_BORDER(p),
                (int)NC_PARENT_Y(d)-HAS_BORDER(p));
        pos_x = BOUND((int)GET_POS_X(d),
                (int)HAS_BORDER(p),
                (int)NC_PARENT_X(d)-HAS_BORDER(p));

        if (STRING(d) || TSTRING(d)) {
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
            int size_y, size_x;
            int bracket;
            int perc = (*(int *)d->data)*100/d->size;
            perc = perc > 100 ? 100 : perc;

            bracket = BB_Y(d) || BB_X(d) ? 1 : 0;
            size_y = BOUND((int)GET_SIZE_Y(d) - 2*bracket, 0,
                    (int)NC_PARENT_Y(d) - 2*HAS_BORDER(p) - 2*bracket);
            size_x = BOUND((int)GET_SIZE_X(d) - 2*bracket, 0,
                    (int)NC_PARENT_X(d) - 2*HAS_BORDER(p) - 2*bracket);

            for (k = 0; k < size_y; k++) {
                wmove(p->win, k + pos_y + bracket, pos_x + bracket);
                for (j = 0; j < size_x; j++) {
                    char *c = " ";
                    if (B_RL(d)) {
                        if ((size_x - j)*100/size_x <= perc)
                            c = "\u258c";
                    }
                    else if (B_LR(d)) {
                        if (j*100/size_x < perc)
                            c = "\u2590";
                    }
                    else if (B_TB(d)) {
                        if (k*100/size_y < perc)
                            c = "\u2580";
                    }
                    else if (B_BT(d)) {
                        if ((size_y - k)*100/size_y <= perc)
                            c = "\u2584";
                    }
                    wprintw(p->win, c);
                }
            }

            /* Bar brackets */
            if (bracket) {
                if (BB_X(d)) {
                    for (k = 1; k < size_y+1; k++) {
                        mvwprintw(p->win, k+pos_y, pos_x, "\u2502");
                        mvwprintw(p->win, k+pos_y, pos_x+size_x+1, "\u2502");
                    }
                }
                if (BB_Y(d)) {
                    for (j = 1; j < size_x+1; j++) {
                        mvwprintw(p->win, pos_y, j+pos_x, "\u2500");
                        mvwprintw(p->win, pos_y+size_y+1, j+pos_x, "\u2500");
                    }
                }
                mvwprintw(p->win, pos_y, pos_x, "\u250c");
                mvwprintw(p->win, pos_y, pos_x+size_x+1, "\u2510");
                mvwprintw(p->win, pos_y+size_y+1, pos_x, "\u2514");
                mvwprintw(p->win, pos_y+size_y+1, pos_x+size_x+1, "\u2518");
            }
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
                 int pos_y, int pos_x,
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
