#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

void help() {
  printf("- press 'c <i> <j>' to rotate piece clockwise in square (i,j)\n");
  printf("- press 'a <i> <j>' to rotate piece anti-clockwise in square (i,j)\n");
  printf("- press 'z' to undo the last move\n");
  printf("- press 'y' to redo the last undone move\n");
  printf("- press 'r' to shuffle game\n");
  printf("- press 's' to save the game\n");
  printf("- press 'q' to quit\n");
}

int main(int argc, char* argv[]) {
  game g = NULL;
  if (argc == 1) {
    g = game_default();
  } else if (argc == 2) {
    g = game_load(argv[1]);
  } else {
    fprintf(stderr, "Too many arguments\n");
    exit(EXIT_FAILURE);
  }
  char ptr;
  uint i, j;
  while (!game_won(g)) {
    game_print(g);
    printf("Entrez une commande : ");
    scanf(" %c", &ptr);

    if (ptr == 'h') {
      help();
    } else if (ptr == 'r') {
      game_shuffle_orientation(g);
    } else if (ptr == 'z') {
      game_undo(g);
      printf("Dernier coup annulé.\n");
    } else if (ptr == 'y') {
      game_redo(g);
      printf("Dernier coup refait.\n");
    } else if (ptr == 'q') {
      game_delete(g);
      printf("shame\n");
      return EXIT_SUCCESS;
    } else if (ptr == 's') {
      char* filename = malloc(sizeof(char) * 100);
      if (filename == NULL) {
        fprintf(stderr, "Not enough memory\n");
      } else {
        printf("Entrez le nom du fichier pour la sauvegarde (50 letters max) : \n");
        if (scanf("%s", filename)) {
          game_save(g, filename);
          free(filename);
        }
      }
    } else if (ptr == 'c' || ptr == 'a') {
      printf("Entrez les coordonnées (i,j) : \n");
      if (scanf("%d %d", &i, &j) == 2) {
        if ((i >= 0 && i < g->nb_rows && (j >= 0 && j < g->nb_cols))) {
          if ((i >= 0 && i < g->nb_rows) && (j >= 0 && j < g->nb_cols)) {
            if (ptr == 'c') {
              game_play_move(g, i, j, 1);
              printf("play move 'c' into square (%d,%d)\n", i, j);
            } else if (ptr == 'a') {
              game_play_move(g, i, j, -1);
              printf("play move 'a' into square (%d,%d)\n", i, j);

            } else {
              printf("Commande invalid !\n");
            }
          }
        }
      }
    }
  }
  printf("congratulation\n");
  game_print(g);
  game_delete(g);
  /*...*/
  return EXIT_SUCCESS;
}