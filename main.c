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
    int data5 = 100;
    int data6 = 100;
    int num_win = 2;
    nc_window wins[num_win];

    setlocale(LC_ALL, "");
    start_curses();

    ncw_init(&wins[0], 0, 0, 50, 100, NCW_BORDER_THN);
    ncw_init(&wins[1], 50, 0, 50, 100, NCW_BORDER_N);

    timeout(20);    // ~50 Hz
    int count = 0;
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

        if (count++ > 50) {
            char *tmp = data1;
            data1 = data2;
            data2 = tmp;
            count = 0;
            data5 = (data5 + 5)%105;
            data6 = (data6 + 10)%110;
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
