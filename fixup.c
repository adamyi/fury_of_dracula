#include <err.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sysexits.h>
#include <unistd.h>
int main(int argc, const char *argv[]) {
  char *past_plays = NULL;
  // fgets(past_plays, MAXL, stdin);
  size_t len;
  getline(&past_plays, &len, stdin);
  // getline's len is wrong sometimes... don't know why
  len = strlen(past_plays);
  while (past_plays[len - 1] != '.' &&
         (past_plays[len - 1] < 'A' || past_plays[len - 1] > 'Z') && len > 0)
    past_plays[--len] = '\0';

  for (int i = 0; i < len;) {
    for (int j = 0; j < 7; i++, j++)
      putchar(past_plays[i]);
    putchar(' ');
  }

  return EXIT_SUCCESS;
}
