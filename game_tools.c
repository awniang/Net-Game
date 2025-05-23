#include "game_tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "game_aux.h"
#include "game_struct.h"
#include "queue.h"
// @copyright University of Bordeaux. All rights reserved, 2024.

/* ************************************************************************** */

/** @brief Hard-coding of pieces (shape & orientation) in an integer array.
 * @details The 4 least significant bits encode the presence of an half-edge in
 * the N-E-S-W directions (in that order). Thus, binary coding 1100 represents
 * the piece "└" (a corner in north orientation).
 */
static uint _code[NB_SHAPES][NB_DIRS] = {
    {0b0000, 0b0000, 0b0000, 0b0000},  // EMPTY {" ", " ", " ", " "}
    {0b1000, 0b0100, 0b0010, 0b0001},  // ENDPOINT {"^", ">", "v", "<"},
    {0b1010, 0b0101, 0b1010, 0b0101},  // SEGMENT {"|", "-", "|", "-"},
    {0b1100, 0b0110, 0b0011, 0b1001},  // CORNER {"└", "┌", "┐", "┘"}
    {0b1101, 0b1110, 0b0111, 0b1011},  // TEE {"┴", "├", "┬", "┤"}
    {0b1111, 0b1111, 0b1111, 0b1111}   // CROSS {"+", "+", "+", "+"}
};

/* ************************************************************************** */

/** encode a shape and an orientation into an integer code */
static uint _encode_shape(shape s, direction o) { return _code[s][o]; }

/* ************************************************************************** */

/** decode an integer code into a shape and an orientation */
static bool _decode_shape(uint code, shape* s, direction* o) {
  assert(code >= 0 && code < 16);
  assert(s);
  assert(o);
  for (int i = 0; i < NB_SHAPES; i++)
    for (int j = 0; j < NB_DIRS; j++)
      if (code == _code[i][j]) {
        *s = i;
        *o = j;
        return true;
      }
  return false;
}

/* ************************************************************************** */

/** add an half-edge in the direction d */
static void _add_half_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  shape s = game_get_piece_shape(g, i, j);
  direction o = game_get_piece_orientation(g, i, j);
  uint code = _encode_shape(s, o);
  uint mask = 0b1000 >> d;     // mask with half-edge in the direction d
  assert((code & mask) == 0);  // check there is no half-edge in the direction d
  uint newcode = code | mask;  // add the half-edge in the direction d
  shape news;
  direction newo;
  bool ok = _decode_shape(newcode, &news, &newo);
  assert(ok);
  game_set_piece_shape(g, i, j, news);
  game_set_piece_orientation(g, i, j, newo);
}

/* ************************************************************************** */

#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)

/* ************************************************************************** */

/**
 * @brief Add an edge between two adjacent squares.
 * @details This is done by modifying the pieces of the two adjacent squares.
 * More precisely, we add an half-edge to each adjacent square, so as to build
 * an edge between these two squares.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param d the direction of the adjacent square
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if an edge can be added, false otherwise
 */
static bool _add_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  uint nexti, nextj;
  bool next = game_get_ajacent_square(g, i, j, d, &nexti, &nextj);
  if (!next) return false;

  // check if the two half-edges are free
  bool he = game_has_half_edge(g, i, j, d);
  if (he) return false;
  bool next_he = game_has_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));
  if (next_he) return false;

  _add_half_edge(g, i, j, d);
  _add_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));

  return true;
}

/* ************************************************************************** */

game game_load(char* filename) {
  FILE* g_file = fopen(filename, "r");
  if (g_file == NULL) {
    exit(EXIT_FAILURE);
  }
  int nb_rows, nb_cols, wrapping;
  if (fscanf(g_file, "%d %d %d\n", &nb_rows, &nb_cols, &wrapping) != 3) {
    exit(EXIT_FAILURE);
  }
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      int c_s = fgetc(g_file);
      int c_d = fgetc(g_file);
      fgetc(g_file);
      if ((c_s != EOF && c_d != EOF)) {
        switch (c_s) {
          case ('E'):
            game_set_piece_shape(g, i, j, EMPTY);
            break;
          case ('N'):
            game_set_piece_shape(g, i, j, ENDPOINT);
            break;
          case ('S'):
            game_set_piece_shape(g, i, j, SEGMENT);
            break;
          case ('C'):
            game_set_piece_shape(g, i, j, CORNER);
            break;
          case ('T'):
            game_set_piece_shape(g, i, j, TEE);
            break;
          case ('X'):
            game_set_piece_shape(g, i, j, CROSS);
            break;
          default:
            exit(EXIT_FAILURE);
        }

        switch (c_d) {
          case ('N'):
            game_set_piece_orientation(g, i, j, NORTH);
            break;
          case ('E'):
            game_set_piece_orientation(g, i, j, EAST);
            break;
          case ('S'):
            game_set_piece_orientation(g, i, j, SOUTH);
            break;
          case ('W'):
            game_set_piece_orientation(g, i, j, WEST);
            break;
          default:
            exit(EXIT_FAILURE);
        }
      }
    }
    fgetc(g_file);
  }
  fclose(g_file);
  return g;
}

void game_save(cgame g, char* filename) {
  if (g == NULL || filename == NULL) {
    exit(EXIT_FAILURE);
  }

  FILE* file = fopen(filename, "w");
  if (file == NULL) {
    exit(EXIT_FAILURE);
  }
  uint nb_cols = game_nb_cols(g), nb_rows = game_nb_rows(g), wrapping = game_is_wrapping(g);
  fprintf(file, "%d %d %d\n", nb_rows, nb_cols, wrapping);

  char tab_dir[] = {'N', 'E', 'S', 'W'};
  char tab_shape[] = {'E', 'N', 'S', 'C', 'T', 'X'};

  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      fprintf(file, "%c%c ", tab_shape[game_get_piece_shape(g, i, j)], tab_dir[game_get_piece_orientation(g, i, j)]);
    }
    fprintf(file, "\n");
  }

  fclose(file);
}

void remove_d(direction d[], direction del_d, uint* size) {
  for (int i = 0; i < *size; i++) {
    if (d[i] == del_d) {
      memmove(&d[i], &d[i + 1], sizeof(direction) * (*size - 1 - i));
      (*size)--;
      return;
    }
  }
}

bool get_empty_case(game g, uint* ii, uint* jj, direction* dd) {
  int offset[NB_DIRS][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
  queue* q_cases = queue_new();
  for (uint i = 0; i < g->nb_rows; i++) {
    for (uint j = 0; j < g->nb_cols; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY) {  // check all not empty shape cases
                                                     // prevent seg err function get piece shape
        direction possible_adj[] = {NORTH, EAST, SOUTH, WEST};
        uint d_size = 4;
        if (i == 0) remove_d(possible_adj, NORTH, &d_size);
        if (i == g->nb_rows - 1) remove_d(possible_adj, SOUTH, &d_size);
        if (j == 0) remove_d(possible_adj, WEST, &d_size);
        if (j == g->nb_cols - 1) remove_d(possible_adj, EAST, &d_size);
        for (uint d = 0; d < d_size; d++) {
          // get all adjacent empty case and push into queue
          if (game_get_piece_shape(g, i + offset[possible_adj[d]][0], j + offset[possible_adj[d]][1]) == EMPTY) {
            uint* has_shape = malloc(sizeof(uint) * 3);
            if (has_shape == NULL) {
              fprintf(stderr, "Not enough memory\n");
              return false;
            }
            has_shape[0] = i;
            has_shape[1] = j;
            has_shape[2] = possible_adj[d];
            queue_push_tail(q_cases, has_shape);
          }
        }
      }
    }
  }
  if (queue_length(q_cases) == 0) {
    queue_free_full(q_cases, free);
    return false;
  };
  // to get a random choice
  uint random_case = rand() % queue_length(q_cases);
  for (uint i = 0; i < random_case; i++) {
    free(queue_pop_head(q_cases));
  }
  // pass the value and free memory
  uint* choosen_case = queue_pop_head(q_cases);
  *ii = choosen_case[0];
  *jj = choosen_case[1];
  *dd = choosen_case[2];
  queue_free_full(q_cases, free);
  free(choosen_case);
  return true;
}
game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_empty, uint nb_extra) {
  if (nb_cols * nb_rows < 2 || nb_empty > (nb_cols * nb_rows - 2) || nb_extra > nb_cols * nb_rows - nb_empty) return NULL;
  // place aléatoirement sur la grille vide un jeu solution à 2 pièces, c'est-à-dire > < horizontal ou
  // vertical
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (g == NULL) return NULL;
  uint nb_cases = nb_cols * nb_rows;
  uint i = (((double)rand() * nb_rows) / RAND_MAX);
  uint j = (((double)rand() * nb_cols) / RAND_MAX);
  uint d;
  // wrapping have more possibility to forme a game
  if (wrapping) {
    d = (((double)rand() * NB_DIRS) / RAND_MAX);
  } else {
    // not wrapping
    direction all_d[] = {NORTH, EAST, SOUTH, WEST};
    uint size = NB_DIRS;
    if (i == 0) remove_d(all_d, NORTH, &size);
    if (i == nb_rows - 1) remove_d(all_d, SOUTH, &size);
    if (j == 0) remove_d(all_d, WEST, &size);
    if (j == nb_cols - 1) remove_d(all_d, EAST, &size);
    d = all_d[rand() % size];
  }
  // initialization
  _add_edge(g, i, j, d);
  nb_cases -= 2;

  // Loop adding edge
  direction p_d;
  uint p_i, p_j;
  while (nb_cases > nb_empty) {
    if (get_empty_case(g, &p_i, &p_j, &p_d)) {
      if (_add_edge(g, p_i, p_j, p_d) && game_is_connected(g)) {
        nb_cases--;
      } else {
        printf("err\n");
        game_delete(g);
        return NULL;
      }
    } else {
      game_delete(g);
      return NULL;
    }
  }
  for (uint k = 0; k < nb_extra; k++) {
    uint extra_i, extra_j, neighbor_i, neighbor_j;
    direction extra_d;
    bool cycle_created = false;

    // Finding edges that can be connected
    for (uint i = 0; i < nb_rows && !cycle_created; i++) {
      for (uint j = 0; j < nb_cols && !cycle_created; j++) {
        if (game_get_piece_shape(g, i, j) != EMPTY) {
          for (uint d = 0; d < NB_DIRS; d++) {
            if (game_get_ajacent_square(g, i, j, d, &neighbor_i, &neighbor_j)) {
              if (game_get_piece_shape(g, neighbor_i, neighbor_j) != EMPTY && !game_has_half_edge(g, i, j, d) &&
                  !game_has_half_edge(g, neighbor_i, neighbor_j, OPPOSITE_DIR(d))) {
                extra_i = i;
                extra_j = j;
                extra_d = d;
                cycle_created = true;
                break;
              }
            }
          }
        }
      }
    }

    if (!cycle_created) {
      printf("Erreur: Cycles could not be created.\n");
      game_delete(g);
      return NULL;
    }

    if (!_add_edge(g, extra_i, extra_j, extra_d)) {
      printf("Erreur: Edge could not be added.\n");
      game_delete(g);
      return NULL;
    }
  }
  return g;
}
bool check(game g, uint pos) {
  uint i = pos / g->nb_cols;
  uint j = pos % g->nb_cols;

  if (game_is_wrapping(g)) {
    if ((i > 0 && game_check_edge(g, i, j, NORTH) == MISMATCH) || (j > 0 && game_check_edge(g, i, j, WEST) == MISMATCH)) return false;
    // Vérifier les bords opposés dans le cas de wrapping
    if ((i == g->nb_rows - 1 && game_check_edge(g, i, j, SOUTH) == MISMATCH) || (j == g->nb_cols - 1 && game_check_edge(g, i, j, EAST) == MISMATCH))
      return false;
  } else {
    if (game_check_edge(g, i, j, NORTH) == MISMATCH || game_check_edge(g, i, j, WEST) == MISMATCH) return false;
  }
  return true;
}

void solve_rec(game g, uint pos, bool onlyfirst, uint size, uint* cpt) {
  if (pos == size) {
    if (game_won(g)) {
      (*cpt)++;
      printf("Solution %d trouvée !\n", *cpt);
      game_print(g);
    }
    return;
  }

  uint i = pos / g->nb_cols;
  uint j = pos % g->nb_cols;

  shape s = game_get_piece_shape(g, i, j);
  direction initial_orientation = game_get_piece_orientation(g, i, j);

  int nb_directions = (s == SEGMENT) ? 2 : (s == CROSS || s == EMPTY) ? 1 : NB_DIRS;

  for (int k = 0; k < nb_directions; k++) {
    direction new_orientation = (initial_orientation + k) % NB_DIRS;
    game_set_piece_orientation(g, i, j, new_orientation);
    if (check(g, pos)) {
      solve_rec(g, pos + 1, onlyfirst, size, cpt);
      if (onlyfirst && *cpt > 0) return;  // Arrêter dès qu'on trouve une solution
    }
  }
  game_set_piece_orientation(g, i, j, initial_orientation);
}

uint game_nb_solutions(cgame g) {
  game g_cpy = game_copy(g);
  uint cpt = 0;
  uint size = g->nb_rows * g->nb_cols;
  printf("Début de la recherche de solutions : %dx%d\n", g->nb_rows, g->nb_cols);
  solve_rec(g_cpy, 0, false, size, &cpt);
  game_delete(g_cpy);
  return cpt;
}

bool game_solve(game g) {
  uint size = g->nb_rows * g->nb_cols;
  uint cpt = 0;
  solve_rec(g, 0, true, size, &cpt);  // Trouver une seule solution
  if (cpt > 0) {
    printf("Une solution a été trouvée !\n");
    return true;
  } else {
    printf("Aucune solution trouvée.\n");
    return false;
  }
}