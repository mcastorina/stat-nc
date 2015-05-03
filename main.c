#define _BSD_SOURCE

#include <locale.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
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

    char *data1 = "Hello, world!";
    char *data2 = "Goodbye      ";
    char *data3 = "\
    _             _     _     _                  \n\
   / \\   _ __ ___| |__ | |   (_)_ __  _   ___  __\n\
  / _ \\ | '__/ __| '_ \\| |   | | '_ \\| | | \\ \\/ /\n\
 / ___ \\| | | (__| | | | |___| | | | | |_| |>  < \n\
/_/   \\_\\_|  \\___|_| |_|_____|_|_| |_|\\__,_/_/\\_\\";

    char *data4 = "\
               +              \n\
               #              \n\
              ###             \n\
             #####            \n\
             ######           \n\
            ; #####;          \n\
           +##.#####          \n\
          +##########         \n\
         #############;       \n\
        ###############+      \n\
       #######   #######      \n\
     .######;     ;###;`\\\".   \n\
    .#######;     ;#####.     \n\
    #########.   .########`   \n\
   ######'           '######  \n\
  ;####                 ####; \n\
  ##'                     '## \n\
 #'                         `#";
    char *data5 = ":)";
    int data6 = 100;
    nc_window wins[3];

    setlocale(LC_ALL, "");
    start_curses();
    ncw_init(&wins[0], 0, 0, false, 50, 100, false, NC_BORDER_N);
    ncw_init(&wins[1], 50, 0, false, 50, 100, false, NC_BORDER_THIN);
    ncw_init(&wins[2], 1, 1, true, 4, 30, true, NC_BORDER_THICK);
    ncw_add_data(&wins[2], (void *)&data1, strlen(data1),
                 1, 1, true, 0, 0, true, 0, 0, NC_STRING);
    ncw_add_data(&wins[2], (void *)&data2, strlen(data2),
                 2, 1, true, 0, 0, true, 0, 0, NC_STRING);
    ncw_add_data(&wins[1], (void *)&data4, strlen(data4),
                 1, 1, false, 0, 0, false, NC_CENTER, NC_LEFT, NC_STRING);
    ncw_add_data(&wins[1], (void *)&data3, strlen(data3),
                 5, 20, false, 0, 0, false, NC_CENTER, NC_CENTER, NC_STRING_T);
    ncw_add_data(&wins[0], (void *)&data5, strlen(data5),
                 80, 80, false, 0, 0, false, -1, -1, NC_STRING);
    ncw_add_data(&wins[0], (void *)&data6, sizeof(data6),
                 10, 50, false, 22, 6, true, NC_RIGHT, NC_RIGHT, NC_VERT);
    ncw_add_data(&wins[1], (void *)&data6, sizeof(data6),
                 10, 50, false, 6, 40, true, NC_LEFT, NC_RIGHT, NC_HORZ);

    timeout(20);    // ~50 Hz
    int count = 0;
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

        if (count++ > 50) {
            char *tmp = data1;
            data1 = data2;
            data2 = tmp;
            count = 0;
            data6 = (data6 + 5)%105;
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
