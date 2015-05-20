#define _DEFAULT_SOURCE

#include <locale.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "nc_window.h"
#include "ncd_string.h"

#define EXIT   (-1)

bool curses_started = false;

void start_curses(void);
void end_curses(void);

// modify window based on input
// returns EXIT if the exit key was pressed (q)
int process_input(char);

int main(int argc, char **argv) {
    int i;

    int num_win = 2;
    nc_window wins[num_win];
    nc_data data1, data2, data3, data4;

    setlocale(LC_ALL, "");
    start_curses();

    ncw_init(&wins[0], 0, 0, 50, 100, NCW_BORDER_THN);
    ncw_init(&wins[1], 50, 0, 50, 100, NCW_BORDER_N);
    ncd_init(&data1, "date", 1000, 0, 0, 0, 0, NC_LEFT | NC_TOP);
    ncd_init(&data2, "./cpu", 500, 1, 0, 6, 26, NC_LEFT | NC_TOP | NC_FIXS_X | NC_FIXS_Y);
    ncd_init(&data3, "cat arch.txt", 0, 0, 0, 18, 29,
            NC_CENTER_X | NC_CENTER_Y | NC_FIXS_X | NC_FIXS_Y);
    ncd_init(&data4, "upower -i /org/freedesktop/UPower/devices/battery_BAT0 |\
                      grep percentage | awk 'NF>1{print \"Battery:\",$NF}'",
                      5000, -1, 0, 0, 17, NC_RIGHT | NC_BOTTOM | NC_FIXS_X);
    ncd_string_init(&data1, 0);
    ncd_string_init(&data2, 0);
    ncd_string_init(&data3, 0);
    ncd_string_init(&data4, 0);
    ncw_add_data(&wins[0], &data1);
    ncw_add_data(&wins[0], &data2);
    ncw_add_data(&wins[1], &data3);
    ncw_add_data(&wins[0], &data4);

    ncw_update();
    NC_WIN_RES &= 0;
    ncw_resize(&wins[0]);
    ncw_resize(&wins[1]);

    timeout(20);    // ~50 Hz
    while (true) {

        if (ncw_update()) {
            if (NC_WIN_RES) {
                for (i = 0; i < num_win; i++) {
                    ncw_resize(&wins[i]);
                }
                NC_WIN_RES &= 0;
            }
        }

        for (i = 0; i < num_win; i++) {
            ncw_draw(&wins[i]);
            wrefresh(wins[i].win);
        }

        int ch = getch();
        if (ch != ERR) {
            int ret = process_input(ch);
            if (ret == EXIT)
                break;
        }
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
