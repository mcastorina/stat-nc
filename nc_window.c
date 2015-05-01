#include "nc_window.h"
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

uint32_t NC_PARENT_Y, NC_PARENT_X, NC_WIN_RES;

int ncd_init(nc_data *p,
             void *data, size_t size,
             uint32_t pos_y, uint32_t pos_x,
             uint32_t size_y, uint32_t size_x,
             int vert_type, int horz_type,
             int type) {
    p->data = data;
    if (size > NC_MAX_DATA_SIZE)
        size = NC_MAX_DATA_SIZE;
    p->size = size;
    if (!(pos_y <= 100 && pos_x <= 100)) {
        pos_y = 0;
        pos_x = 0;
    }
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    if (!(size_y <= 100 && size_x <= 100)) {
        size_y = 100;
        size_x = 100;
    }
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
             int border,
             nc_data **data, size_t data_size) {

    uint32_t new_size_y, new_size_x;
    uint32_t new_pos_y, new_pos_x;

    p->win = malloc(sizeof(WINDOW));
    if (p->win == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
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
        p->fixed &= ~1;
    }
    if (!fixed_size) {
        if (!(size_y <= 100 && size_x <= 100)) {
            size_y = 100;
            size_x = 100;
        }
        new_size_y = NC_PARENT_Y*size_y/100;
        new_size_x = NC_PARENT_X*size_x/100;
        p->fixed &= ~2;
    }
    p->win = newwin(new_size_y, new_size_x, new_pos_y, new_pos_x);
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    p->size_y = size_y;
    p->size_x = size_x;
    p->border = border;
    p->data = data;
    p->data_size = data_size;
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
    new_y = NCW_FIXED_SIZE(p) ? p->size_y : NC_PARENT_Y*p->size_y/100;
    new_x = NCW_FIXED_SIZE(p) ? p->size_x : NC_PARENT_X*p->size_x/100;
    wresize(p->win, new_y, new_x);
    new_y = NCW_FIXED_POS(p) ? p->pos_y : NC_PARENT_Y*p->pos_y/100;
    new_x = NCW_FIXED_POS(p) ? p->pos_x : NC_PARENT_X*p->pos_x/100;
    mvwin(p->win, new_y, new_x);

    wclear(stdscr);
    wclear(p->win);

    return 0;
}

int ncw_draw(nc_window *p) {

    /* Draw border last */
    int i;
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

