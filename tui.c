#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ARR_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define BUFFER 1024

int main() {
  char *words[] = {"ciao", "luca", "come", "va"};
  char input[BUFFER];
  size_t selectline = 0;
  size_t inputpos = 0;
  int row, col;
  int c;
  bool run = true;

  initscr();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);

  while (run) {
    clear();
    getmaxyx(stdscr, row, col);

    for (size_t i = 0; i < ARR_SIZE(words); i++) {
      if (selectline == i) {
        attron(A_STANDOUT);
      }
      mvprintw(i, 0, "%s", words[i]);
      if (selectline == i) {
        attroff(A_STANDOUT);
      }
    }
    // clearinput
    char *clearinput = malloc(sizeof(char) * col + 1);
    memset(clearinput, ' ', col);
    clearinput[col + 1] = '\0';
    mvprintw(row - 1, 0, "%s", clearinput);
    free(clearinput);
    // input
    mvprintw(row - 1, 0, "%s", input);
    refresh();

    c = getch();
    switch (c) {
    case KEY_UP:
      if (selectline > 0) {
        selectline--;
      }
      break;
    case KEY_DOWN:
      if (selectline < ARR_SIZE(words) - 1) {
        selectline++;
      }
      break;
    case 27: // esc
      run = false;
      break;
    case ('\t'):
      NULL;
      break;
    case ('\n'):
      NULL;
      break;
    case KEY_BACKSPACE:
      if (inputpos > 0) {
        inputpos--;
        input[inputpos] = '\0';
      }
      break;
    default:
      if (inputpos < BUFFER - 1) {
        input[inputpos] = c;
        inputpos++;
        input[inputpos] = '\0';
      }
      break;
    }
  }

  endwin();
  return 0;
}