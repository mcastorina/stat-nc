#define _BSD_SOURCE

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

#define EXIT   (-1)

bool curses_started = false;
int parent_x, parent_y;         // window dimensions

void start_curses(void);
void end_curses(void);

// modify window based on input
// returns EXIT if the exit key was pressed (q)
int process_input(char);

// draws border on window
void win_border(WINDOW *);

// resizes window to new_y and new_x
void win_resize(WINDOW *, int new_y, int new_x); 


int main(int argc, char **argv) {
    int new_x, new_y;
    setlocale(LC_ALL, "");
    start_curses();

    WINDOW *win = newwin(parent_y, parent_x, 0, 0);

    timeout(25);
    while (true) {

        getmaxyx(stdscr, new_y, new_x);
        if (new_y != parent_y || new_x != parent_x) {
            win_resize(win, new_y, new_x);
            parent_y = new_y;
            parent_x = new_x;
        }


        win_border(win);
        wrefresh(win);

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
    addch(c);
    return 0;
}
void win_resize(WINDOW *win, int new_y, int new_x) {
    wresize(win, new_y, new_x);

    wclear(stdscr);
    wclear(win);
}
void win_border(WINDOW *screen) {
    int x, y, i;

    getmaxyx(screen, y, x);

    mvwprintw(screen, 0, 0, "\u250c");
    mvwprintw(screen, y-1, 0, "\u2514");
    mvwprintw(screen, 0, x-1, "\u2510");
    mvwprintw(screen, y-1, x-1, "\u2518");

    for (i = 1; i < x-1; i++) {
        mvwprintw(screen, 0, i, "\u2500");
        mvwprintw(screen, y-1, i, "\u2500");
    }
    for (i = 1; i < y-1; i++) {
        mvwprintw(screen, i, 0, "\u2502");
        mvwprintw(screen, i, x-1, "\u2502");
    }
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
        atexit(end_curses);
        curses_started = true;
    }
    getmaxyx(stdscr, parent_y, parent_x);
}
