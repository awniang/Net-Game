#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_tools.h"

void usage(char* cmd) {
  printf("Usage: %s <nb_rows> <nb_cols> <wrapping> <nb_empty> <nb_extra> <shuffle> [<filename>]\n", cmd);
  printf("Example: %s 4 4 0 0 0 0 random.sol\n", cmd);
}
int main(int argc, char* argv[]) {
  if (argc < 7 || argc > 8) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  int nb_rows = atoi(argv[1]);
  int nb_cols = atoi(argv[2]);
  int wrapping = atoi(argv[3]);
  int nb_empty = atoi(argv[4]);
  int nb_extra = atoi(argv[5]);
  int shuffle = atoi(argv[6]);
  char* filename = NULL;
  if (argc == 8) {
    filename = argv[7];
  }
  srand(time(NULL));
  game g = game_random(nb_rows, nb_cols, wrapping, nb_empty, nb_extra);
  if (g == NULL) {
    fprintf(stderr, "Error: Cannot generate game_random.\n");
    exit(EXIT_FAILURE);
  }
  if (shuffle) {
    game_shuffle_orientation(g);
  }
  game_print(g);
  if (filename != NULL) {
    game_save(g, filename);
    fprintf(stderr, "Game saved to %s.\n", filename);
  }
  game_delete(g);
}