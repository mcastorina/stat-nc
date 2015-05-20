#include <stdint.h>
#include <ncurses.h>
#include <string.h>
#include "ncd_string.h"
#include "nc_window.h"

void ncd_string_draw(nc_data *ptr, WINDOW *win, char *data);

int ncd_string_init(nc_data *d, uint8_t flags) {
    d->child_flags = flags;

    /* Install draw function */
    d->draw = ncd_string_draw;

    return 0;
}

void ncd_string_draw(nc_data *p, WINDOW *win, char *data) {
    char *token;
    int k, x, i;

    token = strtok(data, "\n");
    k = 0;
    while (token) {
        mvwprintw(win, p->apos_y + k++, p->apos_x, "%s", token);
        x = getcurx(win);
        if (p->apos_x + p->asize_x > x) {
            for (i = 0; i < p->apos_x + p->asize_x - x; i++)
                waddch(win, ' ');
        }
        token = strtok(NULL, "\n");
    }
    while (k < p->asize_y) {
        wmove(win, p->apos_y + k++, p->apos_x);
        for (i = 0; i < p->asize_x; i++)
            waddch(win, ' ');
    }
}
