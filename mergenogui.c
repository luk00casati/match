#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void listdirr(FILE *file, const char *dirpath, const bool dotfiles) {
  DIR *dir;
  struct dirent *entry;

  if (!(dir = opendir(dirpath))) {
    fprintf(stderr, "ERROR: unable to open directory %s\n", dirpath);
    exit(EXIT_FAILURE);
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
      fprintf(file, "%s/\n", path);
      listdirr(file, path, dotfiles);
    } else {
      char filepath[1024];
      snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);
      fprintf(file, "%s\n", filepath);
    }
  }
  closedir(dir);
}

int main(int argc, char *argv[]) {
  FILE *fpathwr = NULL;
  fpathwr = fopen("/home/luca/documents/c-code/fuzzy/path.txt", "w+");
  if (fpathwr == NULL) {
    fprintf(stderr, "ERROR unable to open path\n");
    return 1;
  }
  listdirr(fpathwr, "/home/luca", false);
  fflush(fpathwr);
  rewind(fpathwr);
  FILE *fmatchw = NULL;
  fmatchw = fopen("/home/luca/documents/c-code/fuzzy/matchres.txt", "w");
  if (fmatchw == NULL) {
    fprintf(stderr, "ERROR unable to open path\n");
    return 1;
  }
  char *entry = NULL;
  char *match = NULL;
  ssize_t bytes_read;
  size_t n = 0;
  const char *pattern = "piatti";
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
  return EXIT_SUCCESS;
}