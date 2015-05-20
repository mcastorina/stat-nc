#include "nc_window.h"
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>

uint32_t NC_WIN_Y, NC_WIN_X, NC_WIN_RES;
char *thk[6] = {"\u250f", "\u2513", "\u2517", "\u251b", "\u2503", "\u2501"};
char *thn[6] = {"\u250c", "\u2510", "\u2514", "\u2518", "\u2502", "\u2500"};

#define TL  (0)
#define TR  (1)
#define BL  (2)
#define BR  (3)
#define VL  (4)
#define HL  (5)

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
    int i, b;

    p->asize_y = GET_SIZE_Y(p, NC_WIN_Y);
    p->asize_x = GET_SIZE_X(p, NC_WIN_X);

    p->apos_y = GET_POS_Y(p, (int)NC_WIN_Y);
    p->apos_x = GET_POS_X(p, (int)NC_WIN_X);

    b = !BORDER_N(p);

    /* Resize data objects */
    for (i = 0; i < p->data_size; i++) {
        nc_data *d = p->data[i];
        d->asize_y = GET_SIZE_Y(d, p->asize_y);
        d->asize_x = GET_SIZE_X(d, p->asize_x);
        if (d->asize_y >= p->asize_y - 2*b)
            d->asize_y = p->asize_y - 2*b;
        if (d->asize_x >= p->asize_x - 2*b)
            d->asize_x = p->asize_x - 2*b;

        d->apos_y = GET_POS_Y(d, (int)p->asize_y) + (TOP_JUST(d) ? b : -b);
        d->apos_x = GET_POS_X(d, (int)p->asize_x) + (LEFT_JUST(d) ? b : -b);
    }

    wresize(p->win, p->asize_y, p->asize_x);
    mvwin(p->win, p->apos_y, p->apos_x);
    wclear(p->win);

    return 0;
}
int ncw_add_data(nc_window *p, nc_data *d) {
    if (p->data_size >= NCW_MAX_ARR_LEN)
        return -1;
    p->data[p->data_size++] = d;
    return 0;
}
int ncw_draw(nc_window *p) {
    char **w;
    int i;

    /* Draw data objects */
    for (i = 0; i < p->data_size; i++) {
        ncd_draw(p->data[i], p->win);
    }

    /* Draw border */
    if (BORDER_N(p))
        return 0;
    if (BORDER_THK(p)) {
        w = thk;
    }
    else {
        // default BORDER_THN
        w = thn;
    }
    mvwprintw(p->win, 0, 0, w[TL]);
    mvwprintw(p->win, p->asize_y-1, 0, w[BL]);
    mvwprintw(p->win, 0, p->asize_x-1, w[TR]);
    mvwprintw(p->win, p->asize_y-1, p->asize_x-1, w[BR]);

    for (i = 1; i < p->asize_x-1; i++) {
        mvwprintw(p->win, 0, i, w[HL]);
        mvwprintw(p->win, p->asize_y-1, i, w[HL]);
    }
    for (i = 1; i < p->asize_y-1; i++) {
        mvwprintw(p->win, i, 0, w[VL]);
        mvwprintw(p->win, i, p->asize_x-1, w[VL]);
    }

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
    p->cmd = ncc;
    p->pos_y = pos_y;
    p->pos_x = pos_x;
    p->size_y = size_y;
    p->size_x = size_x;
    p->flags = flags;

    return 0;
}
int ncd_draw(nc_data *p, WINDOW *win) {
    char buf[NCC_BUFFER_SIZE];
    if (ncc_get(p->cmd, buf))
        return -1;
    p->draw(p, win, buf);
    return 0;
}
