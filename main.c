#define _BSD_SOURCE

#include <locale.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include "nc_window.h"

#define EXIT   (-1)

bool curses_started = false;

void start_curses(void);
void end_curses(void);

// modify window based on input
// returns EXIT if the exit key was pressed (q)
int process_input(char);

int main(int argc, char **argv) {
    int i;
    nc_window wins[3];

    setlocale(LC_ALL, "");
    start_curses();
    ncw_init(&wins[0], 0, 0, 50, 100, NC_BORDER_THIN, NULL, 0);
    ncw_init(&wins[1], 50, 0, 50, 100, NC_BORDER_THIN, NULL, 0);
    ncw_init(&wins[2], 25, 48, 50, 4, NC_BORDER_THICK, NULL, 0);

    timeout(25);
    while (true) {

        if (ncw_update()) {
            if (NC_WIN_RES) {
                for (i = 0; i < 3; i++) {
                    ncw_resize(&wins[i]);
                }
                NC_WIN_RES &= 0;
            }
        }

        for (i = 0; i < 3; i++) {
            ncw_draw(&wins[i]);
            wrefresh(wins[i].win);
        }

        int ch = getch();
        if (ch != ERR) {
            int ret = process_input(ch);
            if (ret == EXIT)
                break;
        }
        usleep(20);
    }
    return 0;
}

int process_input(char c) {
    if (c == 'q')
        return EXIT;
    return 0;
}
void end_curses() {
    if (curses_started && !isendwin())
        endwin();
}
void start_curses() {
    if (curses_started) {
        refresh();
    } else {
        initscr();
        cbreak();
        noecho();
        intrflush(stdscr, false);
        keypad(stdscr, true);
        curs_set(0);
        start_color();
        use_default_colors();
        atexit(end_curses);
        curses_started = true;
    }
}
