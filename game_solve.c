#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_tools.h"

void usage(char* cmd) {
  printf("Usage: %s<option> <input> [<output>]\n", cmd);
  printf("Example: %s -s game.txt res.txt\n", cmd);
}

int main(int argc, char* argv[]) {
  if (argc < 3 || argc > 4) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  char* option = argv[1];
  char* input = argv[2];
  char* output = NULL;
  if (argc == 4) {
    output = argv[3];
  }
  game g = game_load(input);

  if (strcmp(option, "-s") == 0) {
    clock_t start = clock();
    bool res_g = game_solve(g);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Temps d'exécution : %.5f secondes\n", time_spent);
    if (res_g && output) {
      game_save(g, output);
    }
    if (!res_g) {
      exit(EXIT_FAILURE);
    }
    game_print(g);
  }

  if (strcmp(option, "-c") == 0) {
    clock_t start = clock();
    int nb_sol = game_nb_solutions(g);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Temps d'exécution : %.5f secondes\n", time_spent);
    if (output) {
      FILE* f = fopen(output, "w");
      if (!f) {
        fprintf(stderr, "Could not open file");
        exit(EXIT_FAILURE);
      }
      fprintf(f, "%d\n", nb_sol);
      fclose(f);
    }
    printf("%d\n", nb_sol);
  }
}