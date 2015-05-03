#include "nc_window.h"
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>

#define HAS_BORDER(p)   (p->border == NC_BORDER_N ? 0 : 1)

uint32_t NC_PARENT_Y, NC_PARENT_X, NC_WIN_RES;

int ncd_init(nc_data *p,
             void *data, size_t size,
             uint32_t pos_y, uint32_t pos_x, int fixed_pos,
             uint32_t size_y, uint32_t size_x, int fixed_size,
             int vert_type, int horz_type,
             int type) {

    p->data = data;
    if (size > NC_MAX_DATA_SIZE)
        size = NC_MAX_DATA_SIZE;
    p->size = size;

    p->fixed = 3;
    if (!fixed_pos) {
        if (!(pos_y <= 100 && pos_x <= 100)) {
            pos_y = 0;
            pos_x = 0;
        }
        p->fixed &= ~(1 << 0);
    }
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    if (!fixed_size) {
        if (!(size_y <= 100 && size_x <= 100)) {
            size_y = 100;
            size_x = 100;
        }
        p->fixed &= ~(1 << 1);
    }
    if (vert_type >= 0)
        p->fixed |= (1 << 2);
    if (horz_type >= 0)
        p->fixed |= (1 << 3);

    p->size_y = size_y;
    p->size_x = size_x;
    p->vert_type = vert_type;
    p->horz_type = horz_type;
    p->type = type;
    return 0;
}

int ncw_init(nc_window *p,
             uint32_t pos_y, uint32_t pos_x, int fixed_pos,
             uint32_t size_y, uint32_t size_x, int fixed_size,
             int border) {

    uint32_t new_size_y, new_size_x;
    uint32_t new_pos_y, new_pos_x;

    p->win = malloc(sizeof(WINDOW));
    if (p->win == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        return -1;
    }
    p->data = malloc(sizeof(nc_data *)*NC_MAX_ARR_LEN);
    if (p->data == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        free(p->win);
        return -1;
    }
    getmaxyx(stdscr, NC_PARENT_Y, NC_PARENT_X);

    new_size_y = size_y;
    new_size_x = size_x;
    new_pos_y = pos_y;
    new_pos_x = pos_x;

    p->fixed = 3;
    if (!fixed_pos) {
        if (!(pos_y <= 100 && pos_x <= 100)) {
            pos_y = 0;
            pos_x = 0;
        }
        new_pos_y = NC_PARENT_Y*pos_y/100;
        new_pos_x = NC_PARENT_X*pos_x/100;
        p->fixed &= ~(1 << 0);
    }
    if (!fixed_size) {
        if (!(size_y <= 100 && size_x <= 100)) {
            size_y = 100;
            size_x = 100;
        }
        new_size_y = NC_PARENT_Y*size_y/100;
        new_size_x = NC_PARENT_X*size_x/100;
        p->fixed &= ~(1 << 1);
    }
    p->win = newwin(new_size_y, new_size_x, new_pos_y, new_pos_x);
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    p->size_y = size_y;
    p->size_x = size_x;
    p->border = border;
    p->data_size = 0;
    return 0;
}

int ncw_update() {
    int new_y, new_x;
    getmaxyx(stdscr, new_y, new_x);
    if (new_y != NC_PARENT_Y || new_x != NC_PARENT_X) {
        NC_PARENT_Y = new_y;
        NC_PARENT_X = new_x;
        NC_WIN_RES |= 1;

        return 1;
    }
    return 0;
}

int ncw_resize(nc_window *p) {
    uint32_t new_y, new_x;
    new_y = NC_FIXED_SIZE(p) ? p->size_y : NC_PARENT_Y*p->size_y/100;
    new_x = NC_FIXED_SIZE(p) ? p->size_x : NC_PARENT_X*p->size_x/100;
    wresize(p->win, new_y, new_x);
    new_y = NC_FIXED_POS(p) ? p->pos_y : NC_PARENT_Y*p->pos_y/100;
    new_x = NC_FIXED_POS(p) ? p->pos_x : NC_PARENT_X*p->pos_x/100;
    mvwin(p->win, new_y, new_x);

    wclear(stdscr);
    wclear(p->win);

    return 0;
}

int ncw_draw(nc_window *p) {
    int i, k;
    int win_y, win_x;   /* Max window dimensions */
    getmaxyx(p->win, win_y, win_x);

    for (i = 0; i < p->data_size; i++) {
        /* Draw each data object */
        nc_data *d = p->data[i];
        if (d->type == NC_STRING || d->type == NC_STRING_T) {
            char str[d->size+1];
            char *token;
            int num_lines = 0;

            /* Wrote my own strncpy to count num_lines at the same time */
            for (k = 0; k < d->size; k++) {
                char c = (*(char **)d->data)[k];
                if (c == '\n')
                    num_lines++;
                str[k] = c;
            }
            str[d->size] = '\0';

            token = strtok(str, "\n");
            k = 0;

            /* Draw line by line */
            while (token) {
                int y = d->pos_y + k;
                int x = d->pos_x;

                /* This chunk is setting up y and x at the right position */
                if (!NC_FIXED_POS(d)) {
                    if (NC_VERT_JUST(d)) {
                        y = win_y - num_lines - 1 - HAS_BORDER(p);
                        if (d->vert_type == NC_CENTER) {
                            y = y/2;
                        }
                        else if (d->vert_type == NC_LEFT) {
                            y = HAS_BORDER(p);
                        }
                        y += k;
                    } else {    // percent
                        y = win_y*d->pos_y/100 + k;
                    }
                    if (NC_HORZ_JUST(d)) {
                        x = win_x - strlen(token) - HAS_BORDER(p);
                        if (d->horz_type == NC_CENTER) {
                            x = x/2;
                        }
                        else if (d->horz_type == NC_LEFT) {
                            x = HAS_BORDER(p);
                        }
                    } else {    // percent
                        x = win_x*d->pos_x/100;
                    }
                }


                /* Transparent type string will skip drawing spaces */
                if (d->type == NC_STRING_T) {
                    int c;
                    wmove(p->win, y, x);
                    for (c = 0; c < strlen(token); c++) {
                        if (token[c] != ' ')
                            waddch(p->win, token[c]);
                        else
                            wmove(p->win, y, x+c+1);
                    }
                } else {
                    mvwprintw(p->win, y, x, token);
                }
                token = strtok(NULL, "\n");
                k++;
            }
        }
        else if (abs(d->type) == NC_VERT || abs(d->type) == NC_HORZ) {
            int y, x;
            int size_y, size_x;
            char block, space;

            block = '#';
            space = ' ';

            /* This chunk is setting up size_y and size_x */
            size_y = d->size_y;
            size_x = d->size_x;
            if (!NC_FIXED_SIZE(d)) {
                size_y = win_y*d->size_y/100;
                size_x = win_x*d->size_x/100;
            }

            /* This chunk is setting up y and x at the right position */
            y = d->pos_y;
            x = d->pos_x;
            if (!NC_FIXED_POS(d)) {
                if (NC_VERT_JUST(d)) {
                    y = win_y - size_y - HAS_BORDER(p);
                    if (d->vert_type == NC_CENTER) {
                        y = y/2;
                    }
                    else if (d->vert_type == NC_LEFT) {
                        y = HAS_BORDER(p);
                    }
                } else {    // percent
                    y = win_y*d->pos_y/100;
                }
                if (NC_HORZ_JUST(d)) {
                    x = win_x - size_x - HAS_BORDER(p);
                    if (d->horz_type == NC_CENTER) {
                        x = x/2;
                    }
                    else if (d->horz_type == NC_LEFT) {
                        x = HAS_BORDER(p);
                    }
                } else {    // percent
                    x = win_x*d->pos_x/100;
                }
            }

            /* Fix after finding position and size */
            y = (y < 0 ? HAS_BORDER(p) : y);
            x = (x < 0 ? HAS_BORDER(p) : x);
            size_y = (y + size_y > win_y) ? win_y - y - HAS_BORDER(p) : size_y;
            size_x = (x + size_x > win_x) ? win_x - x - HAS_BORDER(p) : size_x;

            /* Laziness */
            if (abs(d->type) == NC_VERT) {
                for (i = 1; i < size_y-1; i++) {
                    wmove(p->win, (d->type == NC_VERT) ? y+i : y+size_y-1-i, x+1);
                    for (k = 0; k < size_x-2; k++) {
                        if (size_y-i-2 < (*(int *)d->data)*size_y/100)
                            //wprintw(p->win, "\u2500");
                            waddch(p->win, block);
                        else
                            waddch(p->win, space);
                    }
                }
            }
            else {
                for (i = 1; i < size_y-1; i++) {
                    wmove(p->win, y+i, x+1);
                    for (k = 0; k < size_x-2; k++) {
                        if (d->type == NC_HORZ ? k < (*(int *)d->data)*size_x/100 :
                                                 size_x-k-2 < (*(int *)d->data)*size_x/100)
                            //wprintw(p->win, "\u2500");
                            waddch(p->win, block);
                        else
                            waddch(p->win, space);
                    }
                }
            }

            /* Corners */
            mvwprintw(p->win, y, x, "\u250c");
            mvwprintw(p->win, y+size_y-1, x, "\u2514");
            mvwprintw(p->win, y, x+size_x-1, "\u2510");
            mvwprintw(p->win, y+size_y-1, x+size_x-1, "\u2518");

            /* Lines */
            if (abs(d->type) == NC_VERT) {
                for (k = 1; k < size_x-1; k++) {
                    mvwprintw(p->win, y, x+k, "\u2500");
                    mvwprintw(p->win, y+size_y-1, x+k, "\u2500");
                }
            }
            else {
                for (k = 1; k < size_y-1; k++) {
                    mvwprintw(p->win, y+k, x, "\u2502");
                    mvwprintw(p->win, y+k, x+size_x-1, "\u2502");
                }
            }
        }
    }

    /* Draw border last */
    char *tl, *tr, *bl, *br;
    char *vl, *hl;
    int max_y, max_x;

    if (p->border == NC_BORDER_N)
        return 0;

    if (p->border == NC_BORDER_THICK) {
        tl = "\u250f";
        tr = "\u2513";
        bl = "\u2517";
        br = "\u251b";

        vl = "\u2503";
        hl = "\u2501";
    }
    else if (p->border == NC_BORDER_DBL) {
        tl = "\u2554";
        tr = "\u2557";
        bl = "\u255a";
        br = "\u255d";

        vl = "\u2551";
        hl = "\u2550";
    }
    else if (p->border == NC_BORDER_DSH) {
        tl = "\u250c";
        tr = "\u2510";
        bl = "\u2514";
        br = "\u2518";

        vl = "\u254e";
        hl = "\u254c";
    } else {  // default NC_BORDER_THIN
        tl = "\u250c";
        tr = "\u2510";
        bl = "\u2514";
        br = "\u2518";

        vl = "\u2502";
        hl = "\u2500";
    }

    getmaxyx(p->win, max_y, max_x);

    mvwprintw(p->win, 0, 0, tl);
    mvwprintw(p->win, max_y-1, 0, bl);
    mvwprintw(p->win, 0, max_x-1, tr);
    mvwprintw(p->win, max_y-1, max_x-1, br);

    for (i = 1; i < max_x-1; i++) {
        mvwprintw(p->win, 0, i, hl);
        mvwprintw(p->win, max_y-1, i, hl);
    }
    for (i = 1; i < max_y-1; i++) {
        mvwprintw(p->win, i, 0, vl);
        mvwprintw(p->win, i, max_x-1, vl);
    }
    return 0;
}

int ncw_add_data(nc_window *p,
                 void *data, size_t size,
                 uint32_t pos_y, uint32_t pos_x, int fixed_pos,
                 uint32_t size_y, uint32_t size_x, int fixed_size,
                 int vert_type, int horz_type,
                 int type) {
    if (p->data_size > NC_MAX_ARR_LEN) {
        fprintf(stderr, "Exceeded maximum allowed data objects.\n");
        return -1;
    }

    nc_data *new_data = malloc(sizeof(nc_data));
    if (new_data == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        return -1;
    }
    ncd_init(new_data, data, size,
             pos_y, pos_x, fixed_pos,
             size_y, size_x, fixed_size,
             vert_type, horz_type, type);
    p->data[p->data_size++] = new_data;
    return 0;
}
