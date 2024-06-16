#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER 1024

int main() {
  char input[BUFFER] = "";
  char selectresult[BUFFER];
  size_t selectline = 0;
  size_t inputpos = 0;
  int row, col;
  int c;
  bool run = true;

  FILE *fptr = fopen("matchres.txt", "r");
  if (fptr == NULL) {
    perror("Error opening file");
    return EXIT_FAILURE;
  }

  initscr();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);

  char *lines[BUFFER];
  size_t line_count = 0;
  size_t n = 0;
  char *line = NULL;
  ssize_t bytes_read;

  while ((bytes_read = getline(&line, &n, fptr)) != -1) {
    line[bytes_read - 1] = '\0';
    lines[line_count] = strdup(line);
    line_count++;
    if (line_count >= BUFFER) {
      break;
    }
  }
  free(line);
  fclose(fptr);

  size_t first_visible_line = 0;

  while (run) {
    clear();
    getmaxyx(stdscr, row, col);

    for (size_t i = 0; i < line_count && i < (size_t)row - 1; i++) {
      size_t display_line = first_visible_line + i;
      if (display_line >= line_count)
        break;

      if (selectline == display_line) {
        attron(A_STANDOUT);
      }
      mvprintw(i, 0, "%s", lines[display_line]);
      if (selectline == display_line) {
        attroff(A_STANDOUT);
      }
    }

    char *clearinput = malloc(sizeof(char) * (col + 1));
    memset(clearinput, ' ', col);
    clearinput[col] = '\0';
    mvprintw(row - 1, 0, "%s", clearinput);
    free(clearinput);

    mvprintw(row - 1, 0, "%s", input);
    refresh();

    c = getch();
    switch (c) {
    case KEY_UP:
      if (selectline > 0) {
        selectline--;
        if (selectline < first_visible_line) {
          first_visible_line--;
        }
      }
      break;
    case KEY_DOWN:
      if (selectline < line_count - 1) {
        selectline++;
        if (selectline >= first_visible_line + row - 1) {
          first_visible_line++;
        }
      }
      break;
    case 27: // esc
      run = false;
      break;
    case '\t':
      NULL;
      break;
    case '\n':
      strcpy(selectresult, lines[selectline]);
      run = false;
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

  for (size_t i = 0; i < line_count; i++) {
    free(lines[i]);
  }

  endwin();
  if (selectline != 0) {
    printf("%s\n", selectresult);
  }
  return 0;
}
