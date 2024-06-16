#include <dirent.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROGRAM_DIR_PATH "/home/luca/mcd/pathres.txt"
#define PROGRAM_DIR_MATCH "/home/luca/mcd/marchres.txt"

#define BUFFER 1024

void listdirr(FILE *file, const char *dirpath, const bool dotfiles) {
  DIR *dir;
  struct dirent *entry;

  if (!(dir = opendir(dirpath))) {
    fprintf(stderr, "ERROR: unable to open directory %s\n", dirpath);
    exit(EXIT_FAILURE);
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      char path[BUFFER];
      if (dotfiles == true) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
          continue;
        }
      }
      if (dotfiles == false) {
        if (strstr(entry->d_name, ".") != NULL) {
          continue;
        }
      }
      snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
      fprintf(file, "%s/\n", path);
      listdirr(file, path, dotfiles);
    } else {
      char filepath[BUFFER];
      snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);
      fprintf(file, "%s\n", filepath);
    }
  }
  closedir(dir);
}

void match(const char *dirpath, const char *pattern) {
  FILE *fpathwr = NULL;
  fpathwr = fopen(PROGRAM_DIR_PATH, "w+");
  if (fpathwr == NULL) {
    fprintf(stderr, "ERROR unable to open path\n");
    exit(EXIT_FAILURE);
  }
  listdirr(fpathwr, dirpath, false);
  fflush(fpathwr);
  rewind(fpathwr);
  FILE *fmatchw = NULL;
  fmatchw = fopen(PROGRAM_DIR_MATCH, "w");
  if (fmatchw == NULL) {
    fprintf(stderr, "ERROR unable to open path\n");
    exit(EXIT_FAILURE);
  }
  char *entry = NULL;
  char *match = NULL;
  ssize_t bytes_read;
  size_t n = 0;
  while ((bytes_read = getline(&entry, &n, fpathwr)) != -1) {
    /* Strip newline included by getline. */
    entry[bytes_read - 1] = '\0';
    match = strcasestr(entry, pattern);
    if (match != NULL) {
      fprintf(fmatchw, "%s\n", entry);
    }
  }
  fclose(fpathwr);
  fclose(fmatchw);
  free(entry);
}

void tui(const char *file) {
  char selectresult[BUFFER];
  size_t selectline = 0;
  int row, col;
  int c;
  bool run = true;
  char *lines[BUFFER];
  size_t line_count = 0;
  size_t n = 0;
  char *line = NULL;
  ssize_t bytes_read;
  size_t first_visible_line = 0;

  FILE *fptr = fopen(file, "r");
  if (fptr == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(EXIT_FAILURE);
  }

  initscr();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);

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

  while (run) {
    clear();
    getmaxyx(stdscr, row, col);

    for (size_t i = 0; i < line_count && i < (size_t)row; i++) {
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
        if (selectline >= first_visible_line + row) {
          first_visible_line++;
        }
      }
      break;
    case 27: // esc
      run = false;
      break;
    case '\n':
      strcpy(selectresult, lines[selectline]);
      run = false;
      break;
    default:
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
  return;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "ERROR too few argumnt -h for help\n");
    return 1;
  }
  if (argc == 2) {
    if (strcmp(argv[1], "-h") == 0) {
      printf("-h for help\n-d for show dot files\n-p for print selected\n");
    } else {
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        match(cwd, argv[1]);
        tui(PROGRAM_DIR_MATCH);
      } else {
        fprintf(stderr, "ERROR fail getcwd\n");
        return 1;
      }
    }
  }
  if (argc == 3) {
    match(argv[1], argv[2]);
    tui(PROGRAM_DIR_MATCH);
  }
  if (argc > 3) {
    fprintf(stderr, "ERROR too many argument\n");
    return 1;
  }
  return 0;
}