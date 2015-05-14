#include <stdint.h>
#include <ncurses.h>
#include "ncd_string.h"
#include "nc_window.h"

void ncd_string_draw(void *ptr, WINDOW *win, char *data);

int ncd_string_init(ncd_string *p, nc_data *d, uint8_t flags) {
    p->parent = d;
    p->flags = flags;

    /* Install child */
    d->ncd_child = (void *)p;
    d->draw = ncd_string_draw;

    return 0;
}

void ncd_string_draw(void *ptr, WINDOW *win, char *data) {
    ncd_string *p = (ncd_string *)ptr;
}
