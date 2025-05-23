#include "assert.h"
#include "game_aux.h"
#include "game_struct.h"
#include "game_tools.h"
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
  assert(g_file != NULL);
  int nb_rows, nb_cols, wrapping;
  assert(fscanf(g_file, "%d %d %d\n", &nb_rows, &nb_cols, &wrapping) == 3);
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  assert(g != NULL);
  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      int c_s = fgetc(g_file);
      int c_d = fgetc(g_file);
      fgetc(g_file);
      if ((c_s != EOF && c_d != EOF)) {
        switch (c_s) {
          case ('E'):
            g->shapes[i][j] = 0;
            break;
          case ('N'):
            g->shapes[i][j] = 1;
            break;
          case ('S'):
            g->shapes[i][j] = 2;
            break;
          case ('C'):
            g->shapes[i][j] = 3;
            break;
          case ('T'):
            g->shapes[i][j] = 4;
            break;
          case ('X'):
            g->shapes[i][j] = 5;
            break;
          default:
            exit(EXIT_FAILURE);
        }

        switch (c_d) {
          case ('N'):
            g->directions[i][j] = 0;
            break;
          case ('E'):
            g->directions[i][j] = 1;
            break;
          case ('S'):
            g->directions[i][j] = 2;
            break;
          case ('W'):
            g->directions[i][j] = 3;
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
  assert(g != NULL && filename != NULL);

  FILE* file = fopen(filename, "w");
  assert(file != NULL);
  uint nb_cols = game_nb_cols(g), nb_rows = game_nb_rows(g), wrapping = game_is_wrapping(g);
  fprintf(file, "%d %d %d\n", nb_rows, nb_cols, wrapping);

  char tab_dir[] = {'N', 'E', 'S', 'W'};
  char tab_shape[] = {'E', 'N', 'S', 'C', 'T', 'X'};

  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      fprintf(file, "%c%c ", tab_shape[g->shapes[i][j]], tab_dir[g->directions[i][j]]);
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
bool solve_rec(game g, uint pos, bool onlyfirst, uint size,
               uint* cpt) {  // onlyfirst permet de compter toutes les solutions si elle est à false  et true
                             // si on veut juste la premiere
  if (pos == size) {
    game_print(g);
    if (game_won(g)) {
      (*cpt)++;
      return true;
    }
    return false;
  }
  uint i = pos / g->nb_cols;
  uint j = pos % g->nb_cols;

  shape s = game_get_piece_shape(g, i, j);
  int nb_directions = NB_DIRS;
  if (s == SEGMENT) {
    nb_directions = 2;
  } else if (s == CROSS || s == EMPTY) {
    nb_directions = 1;
  }
  // printf("Position: (%d, %d), Shape: %d, Directions possibles: %d\n", i, j, s, nb_directions);
  direction before = game_get_piece_orientation(g, i, j);  // on recupère l'orientation de base

  for (int k = 0; k < nb_directions; k++) {
    /*int new_i = 0;
    int new_j = 0;
    if (g->wrapping) {
      if (i == 0 && k == WEST) {
        new_i = g->nb_cols - 1;
      } else if (j == 0 && k == NORTH) {
        new_j = g->nb_rows - 1;
      } else if (i == g->nb_cols - 1 && k == EAST) {
        new_i = 0;
      } else if (j == g->nb_rows - 1 && k == SOUTH) {
        new_j = 0;
      }
    }
      */
    // edge_status status = game_check_edge(g, i, j, k);
    /* if ((i == 0 && (k == WEST || k == NORTH)) || (j == 0 && k == NORTH)) {
       continue;  // Orientation impossible si on est sur le bord gauche ou superieur
     }*/
    /*if (status == MISMATCH) {
      // printf("Mismatch détecté à (%d, %d) pour la direction %d\n", i, j, k);
      continue;
    }
      */
    game_set_piece_orientation(g, i, j, k);
    solve_rec(g, pos + 1, onlyfirst, size, cpt);
    /*if (found && onlyfirst) {
      return true;  // stoppe la récursion si onlyfirst == true
    }*/
    game_set_piece_orientation(g, i, j, before);  // on annule l'orientation et on remet l'ancienne
  }
  return *cpt != 0;
}
uint game_nb_solutions(cgame g) {
  game cpy_g = game_copy(g);
  uint cpt = 0;
  uint size = g->nb_rows * g->nb_cols;
  printf("Début du jeu: %dx%d\n", g->nb_rows, g->nb_cols);
  solve_rec(cpy_g, 0, false, size, &cpt);
  game_delete(cpy_g);
  return cpt;
}

bool game_solve(game g) {
  uint size = g->nb_rows * g->nb_cols;
  game cpy_g = game_copy(g);
  uint cpt = 0;
  bool solution = solve_rec(cpy_g, 0, true, size, &cpt);  // Se limite à la première solution

  if (solution) {
    g = cpy_g;
    printf("Solution trouvée !\n");
    game_print(g);
  } else {
    printf("Aucune solution trouvée.\n");
  }
  game_delete(cpy_g);
  return solution;
}
