#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *fread = NULL;
  fread = fopen("/home/luca/documents/c-code/fuzzy/rout.txt", "r");
  if (fread == NULL) {
    fprintf(stderr, "ERROR unable to open path\n");
    return 1;
  }
  char *entry = NULL;
  char *match = NULL;
  ssize_t bytes_read;
  size_t n = 0;
  const char *pattern = "piatti";
  while ((bytes_read = getline(&entry, &n, fread)) != -1) {
    /* Strip newline included by getline. */
    entry[bytes_read - 1] = '\0';
    match = strcasestr(entry, pattern);
    if (match != NULL){
    printf("%s\n", entry);
    }
  }
  fclose(fread);
  free(entry);
  return EXIT_SUCCESS;
}
