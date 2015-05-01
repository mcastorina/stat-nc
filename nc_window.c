#include "nc_window.h"
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

uint32_t NCW_PARENT_Y, NCW_PARENT_X;

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
    if (!(pos_y >= 0 && pos_y <= 100 && pos_x >= 0 && pos_x <= 100)) {
        pos_y = 0;
        pos_x = 0;
    }
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    if (!(size_y >= 0 && size_y <= 100 && size_x >= 0 && size_x <= 100)) {
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
             uint32_t pos_y, uint32_t pos_x,
             uint32_t size_y, uint32_t size_x,
             int border,
             nc_data **data, size_t data_size) {
    p->win = malloc(sizeof(WINDOW));
    if (p->win == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        return -1;
    }
    p->win = newwin(size_y, size_x, pos_y, pos_x);
    p->size_y = size_y;
    p->size_x = size_x;
    p->border = border;
    p->data = data;
    p->data_size = data_size;
    return 0;
}

int ncw_resize(nc_window *p,
            uint32_t new_y, uint32_t new_x) {
    return 0;
}

int ncw_draw(nc_window *p) {

    /* Draw border last */
    int i;
    char *tl, *tr, *bl, *br;
    char *vl, *hl;

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
    else {  // default NC_BORDER_THIN
        tl = "\u250c";
        tr = "\u2510";
        bl = "\u2514";
        br = "\u2518";

        vl = "\u2502";
        hl = "\u2500";
    }

    mvwprintw(p->win, 0, 0, tl);
    mvwprintw(p->win, p->size_y-1, 0, bl);
    mvwprintw(p->win, 0, p->size_x-1, tr);
    mvwprintw(p->win, p->size_y-1, p->size_x-1, br);

    for (i = 1; i < p->size_x-1; i++) {
        mvwprintw(p->win, 0, i, hl);
        mvwprintw(p->win, p->size_y-1, i, hl);
    }
    for (i = 1; i < p->size_y-1; i++) {
        mvwprintw(p->win, i, 0, vl);
        mvwprintw(p->win, i, p->size_x-1, vl);
    }
    return 0;
}

