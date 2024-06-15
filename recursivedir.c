#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void listdirr(const char *dirpath) {
  DIR *dir;
  struct dirent *entry;
  bool dotfiles = false;

  if (!(dir = opendir(dirpath))) {
    fprintf(stderr, "ERROR: unable to open directory %s\n", dirpath);
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      char path[1024];
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
      printf("%s/\n", path);
      listdirr(path);
    } else {
      char filepath[1024];
      snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);
      printf("%s\n", filepath);
    }
  }
  closedir(dir);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      listdirr(cwd);
    } else {
      fprintf(stderr, "ERROR fail getcwd\n");
      return 1;
    }
  }
  if (argc == 2) {
    if (strcmp(argv[1], "-h") == 0) {
      printf("-h for help\n-d for show dot files\n");
    } else {
      listdirr(argv[1]);
    }
  }
  if (argc == 3) {
    listdirr(argv[2]);
  }
  if (argc > 3) {
    fprintf(stderr, "ERROR too many argument\n");
    return 1;
  }
  return 0;
}
