#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

int test_famseye_dummy() { return EXIT_SUCCESS; }

// Test de la récupération des differentes formes
bool test_famseye_get_piece_shape() {
  game g = game_new_empty();
  assert(g);
  game_set_piece_shape(g, 0, 0, SEGMENT);
  game_set_piece_orientation(g, 0, 0, EAST);

  game_set_piece_shape(g, 2, 2, CORNER);
  game_set_piece_orientation(g, 2, 2, WEST);

  if (game_get_piece_shape(g, 0, 0) != SEGMENT || game_get_piece_shape(g, 2, 2) != CORNER) {
    game_delete(g);
    return false;
  }

  game_delete(g);

  game g1 = game_new_empty_ext(4, 4, false);
  assert(g1);
  game_set_piece_shape(g1, 0, 0, SEGMENT);
  game_set_piece_orientation(g1, 0, 0, EAST);

  game_set_piece_shape(g1, 2, 2, CORNER);
  game_set_piece_orientation(g1, 2, 2, WEST);

  if (game_get_piece_shape(g1, 0, 0) != SEGMENT || game_get_piece_shape(g1, 2, 2) != CORNER) {
    game_delete(g1);
    return false;
  }

  game_delete(g1);

  game g3 = game_new_empty_ext(3, 3, true);
  assert(g3);
  game_set_piece_shape(g3, 1, 1, CROSS);
  game_set_piece_orientation(g3, 1, 1, NORTH);
  if (game_get_piece_shape(g3, 1, 1) != CROSS) {
    game_delete(g3);
    return false;
  }
  game_delete(g3);

  return true;
}

bool test_famseye_game_is_connected() {
  // Test de la connectivité du jeu par défaut
  game g = game_default_solution();
  assert(g);

  if (!game_is_connected(g)) {
    game_delete(g);
    return false;
  }

  game_play_move(g, 0, 0, 1);
  if (game_is_connected(g)) {
    game_delete(g);
    return false;
  }

  game_delete(g);

  game g2 = game_new_empty();

  // Créer un carré en haut à gauche (réseau connecté)
  game_set_piece_shape(g2, 0, 0, CORNER);
  game_set_piece_orientation(g2, 0, 0, EAST);
  game_set_piece_shape(g2, 0, 1, CORNER);
  game_set_piece_orientation(g2, 0, 1, SOUTH);
  game_set_piece_shape(g2, 1, 0, CORNER);
  game_set_piece_orientation(g2, 1, 0, NORTH);
  game_set_piece_shape(g2, 1, 1, CORNER);
  game_set_piece_orientation(g2, 1, 1, WEST);

  // créer un carré en bas à droite (réseau connecté)
  game_set_piece_shape(g2, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, CORNER);
  game_set_piece_orientation(g2, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, WEST);
  game_set_piece_shape(g2, DEFAULT_SIZE - 1, DEFAULT_SIZE - 2, CORNER);
  game_set_piece_orientation(g2, DEFAULT_SIZE - 1, DEFAULT_SIZE - 2, NORTH);
  game_set_piece_shape(g2, DEFAULT_SIZE - 2, DEFAULT_SIZE - 1, CORNER);
  game_set_piece_orientation(g2, DEFAULT_SIZE - 2, DEFAULT_SIZE - 1, SOUTH);
  game_set_piece_shape(g2, DEFAULT_SIZE - 2, DEFAULT_SIZE - 2, CORNER);
  game_set_piece_orientation(g2, DEFAULT_SIZE - 2, DEFAULT_SIZE - 2, EAST);

  bool result = game_is_connected(g2);

  // Deux sous-réseaux non connectés ne doivent pas être gagnants
  if (result == true) {
    return false;
  }
  game_delete(g2);

  // Un réseau connecté avec des cases vides est gagnant
  game g3 = game_new_empty();
  game_set_piece_shape(g3, 0, 0, SEGMENT);
  game_set_piece_orientation(g3, 0, 0, NORTH);
  game_set_piece_shape(g3, 0, 1, SEGMENT);
  game_set_piece_orientation(g3, 0, 1, NORTH);
  game_set_piece_shape(g3, 4, 0, SEGMENT);
  game_set_piece_orientation(g3, 4, 0, NORTH);
  game_set_piece_shape(g3, 4, 1, SEGMENT);
  game_set_piece_orientation(g3, 4, 1, NORTH);

  result = game_is_connected(g3);

  if (result == true) {
    return false;
  }

  game_delete(g3);

  return true;
}

// Test de la récupération des differentes orientations
bool test_famseye_get_piece_orientation() {
  game g = game_new_empty();
  assert(g);
  game_set_piece_shape(g, 0, 0, SEGMENT);
  game_set_piece_orientation(g, 0, 0, EAST);

  game_set_piece_shape(g, 2, 2, CORNER);
  game_set_piece_orientation(g, 2, 2, WEST);

  if (game_get_piece_orientation(g, 0, 0) != EAST || game_get_piece_orientation(g, 2, 2) != WEST) {
    game_delete(g);
    return false;
  }
  game_delete(g);

  game g1 = game_new_empty_ext(4, 4, true);
  assert(g1);
  game_set_piece_shape(g1, 0, 0, CROSS);
  game_set_piece_orientation(g1, 0, 0, EAST);

  game_set_piece_shape(g1, 2, 2, CORNER);
  game_set_piece_orientation(g1, 2, 2, WEST);

  if (game_get_piece_orientation(g1, 0, 0) != EAST || game_get_piece_orientation(g1, 2, 2) != WEST) {
    game_delete(g1);
    return false;
  }
  game_delete(g1);
  return true;
}

// Vérification du résultat de play move pour chaque direction
bool test_famseye_play_move() {
  game g = game_new_empty();
  assert(g);

  game_set_piece_shape(g, 0, 0, SEGMENT);
  game_set_piece_orientation(g, 0, 0, EAST);

  game_set_piece_shape(g, 0, 1, SEGMENT);
  game_set_piece_orientation(g, 0, 1, NORTH);

  game_set_piece_shape(g, 1, 0, SEGMENT);
  game_set_piece_orientation(g, 1, 0, SOUTH);

  game_set_piece_shape(g, 1, 1, SEGMENT);
  game_set_piece_orientation(g, 1, 1, WEST);

  game_play_move(g, 0, 0, 1);
  game_play_move(g, 0, 1, 1);
  game_play_move(g, 1, 0, 1);
  game_play_move(g, 1, 1, 1);

  if (game_get_piece_orientation(g, 0, 0) != SOUTH) {
    game_delete(g);
    return false;
  }

  if (game_get_piece_orientation(g, 0, 1) != EAST) {
    game_delete(g);
    return false;
  }

  if (game_get_piece_orientation(g, 1, 0) != WEST) {
    game_delete(g);
    return false;
  }

  if (game_get_piece_orientation(g, 1, 1) != NORTH) {
    game_delete(g);
    return false;
  }

  game_play_move(g, 0, 0, -1);
  game_play_move(g, 0, 1, -1);
  game_play_move(g, 1, 0, -1);
  game_play_move(g, 1, 1, -1);

  if (game_get_piece_orientation(g, 0, 0) != EAST) {
    game_delete(g);
    return false;
  }

  if (game_get_piece_orientation(g, 0, 1) != NORTH) {
    game_delete(g);
    return false;
  }

  if (game_get_piece_orientation(g, 1, 0) != SOUTH) {
    game_delete(g);
    return false;
  }

  if (game_get_piece_orientation(g, 1, 1) != WEST) {
    game_delete(g);
    return false;
  }

  game_delete(g);

  game g2 = game_new_empty_ext(3, 3, true);
  assert(g2);
  game_set_piece_shape(g2, 0, 2, CROSS);
  game_set_piece_orientation(g2, 0, 2, NORTH);
  game_play_move(g2, 0, 2, 1);
  if (game_get_piece_orientation(g2, 0, 2) != EAST) {
    game_delete(g2);
    return false;
  }
  game_play_move(g2, 0, 2, 1);
  if (game_get_piece_orientation(g2, 0, 2) != SOUTH) {
    game_delete(g2);
    return false;
  }
  game_play_move(g2, 0, 2, 1);
  if (game_get_piece_orientation(g2, 0, 2) != WEST) {
    game_delete(g2);
    return false;
  }
  game_play_move(g2, 0, 2, 1);
  if (game_get_piece_orientation(g2, 0, 2) != NORTH) {
    game_delete(g2);
    return false;
  }
  game_delete(g2);

  game g3 = game_new_empty_ext(3, 3, true);
  assert(g3);
  game_set_piece_shape(g3, 0, 2, CROSS);
  game_set_piece_orientation(g3, 0, 2, NORTH);
  game_play_move(g3, 0, 2, -1);
  if (game_get_piece_orientation(g3, 0, 2) != WEST) {
    game_delete(g3);
    return false;
  }
  game_play_move(g3, 0, 2, -1);
  if (game_get_piece_orientation(g3, 0, 2) != SOUTH) {
    game_delete(g3);
    return false;
  }
  game_play_move(g3, 0, 2, -1);
  if (game_get_piece_orientation(g3, 0, 2) != EAST) {
    game_delete(g3);
    return false;
  }
  game_play_move(g3, 0, 2, -1);
  if (game_get_piece_orientation(g3, 0, 2) != NORTH) {
    game_delete(g3);
    return false;
  }

  game_delete(g3);
  return true;
}

// Vérification que reset remet tout à north
bool test_famseye_game_reset_orientation() {
  game g = game_default();
  assert(g);

  game_reset_orientation(g);

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_piece_orientation(g, i, j) != NORTH) {
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);

  game g1 = game_new_empty_ext(3, 4, true);
  assert(g1);

  game_set_piece_orientation(g1, 0, 0, EAST);
  game_set_piece_orientation(g1, 1, 2, SOUTH);
  game_set_piece_orientation(g1, 2, 3, WEST);

  game_reset_orientation(g1);

  for (int i = 0; i < game_nb_rows(g1); i++) {
    for (int j = 0; j < game_nb_cols(g1); j++) {
      if (game_get_piece_orientation(g1, i, j) != NORTH) {
        game_delete(g1);
        return false;
      }
    }
  }

  game_delete(g1);

  return true;
}

// Verification que shuffle donne un resultat différent à chaque coup
bool test_famseye_game_shuffle_orientation() {
  game g = game_default();
  assert(g);

  game_shuffle_orientation(g);

  direction initial_orientations[DEFAULT_SIZE][DEFAULT_SIZE];

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      initial_orientations[i][j] = game_get_piece_orientation(g, i, j);
    }
  }

  for (int i = 0; i < 3; i++) {
    game_shuffle_orientation(g);

    bool orientations_changed = false;

    for (int x = 0; x < DEFAULT_SIZE; x++) {
      for (int y = 0; y < DEFAULT_SIZE; y++) {
        direction current_orientation = game_get_piece_orientation(g, x, y);
        if (current_orientation != initial_orientations[x][y]) {
          orientations_changed = true;
          break;
        }
      }
      if (orientations_changed) {
        break;
      }
    }

    if (orientations_changed) {
      game_delete(g);
      return true;
    }
  }

  game_delete(g);

  game g1 = game_new_empty_ext(3, 4, true);
  assert(g1);

  game_shuffle_orientation(g1);

  direction initial_orientations2[3][4];

  for (int i = 0; i < game_nb_rows(g1); i++) {
    for (int j = 0; j < game_nb_cols(g1); j++) {
      initial_orientations2[i][j] = game_get_piece_orientation(g, i, j);
    }
  }

  for (int i = 0; i < 3; i++) {
    game_shuffle_orientation(g1);

    bool orientations_changed = false;

    for (int x = 0; x < game_nb_rows(g1); x++) {
      for (int y = 0; y < game_nb_cols(g1); y++) {
        direction current_orientation = game_get_piece_orientation(g, x, y);
        if (current_orientation != initial_orientations2[x][y]) {
          orientations_changed = true;
          break;
        }
      }
      if (orientations_changed) {
        break;
      }
    }

    if (orientations_changed) {
      game_delete(g1);
      return true;
    }
  }

  game_delete(g1);
  return false;
}

bool test_famseye_game_won() {
  // Solution par défaut doit etre gagnante
  game g1 = game_default_solution();
  assert(g1 != NULL);
  if (!game_won(g1)) {
    game_delete(g1);
    return false;
  }
  game_delete(g1);

  // Une modification du jeu par défaut casse la connectivité
  game g2 = game_default_solution();
  assert(g2 != NULL);
  game_play_move(g2, 0, 0, 1);
  if (game_won(g2)) {
    game_delete(g2);
    return false;
  }
  game_delete(g2);

  game g3 = game_new_empty();
  assert(g3 != NULL);
  // Créer deux réseaux connectés séparés
  game_set_piece_shape(g3, 0, 0, CORNER);
  game_set_piece_orientation(g3, 0, 0, EAST);
  game_set_piece_shape(g3, 0, 1, CORNER);
  game_set_piece_orientation(g3, 0, 1, SOUTH);
  game_set_piece_shape(g3, 1, 0, CORNER);
  game_set_piece_orientation(g3, 1, 0, NORTH);
  game_set_piece_shape(g3, 1, 1, CORNER);
  game_set_piece_orientation(g3, 1, 1, WEST);
  game_set_piece_shape(g3, 3, 3, CORNER);
  game_set_piece_orientation(g3, 3, 3, WEST);
  game_set_piece_shape(g3, 3, 2, CORNER);
  game_set_piece_orientation(g3, 3, 2, NORTH);
  game_set_piece_shape(g3, 2, 3, CORNER);
  game_set_piece_orientation(g3, 2, 3, SOUTH);
  game_set_piece_shape(g3, 2, 2, CORNER);
  game_set_piece_orientation(g3, 2, 2, EAST);

  // Le jeu ne doit pas etre gagant car non connecté
  if (game_won(g3)) {
    game_delete(g3);
    return false;
  }
  game_delete(g3);

  // jeu avec un seul réseau mais avec des cases vides est gagnant
  game g4 = game_new_empty();
  assert(g4 != NULL);
  game_set_piece_shape(g4, 0, 0, CORNER);
  game_set_piece_orientation(g4, 0, 0, EAST);
  game_set_piece_shape(g4, 0, 1, CORNER);
  game_set_piece_orientation(g4, 0, 1, SOUTH);
  game_set_piece_shape(g4, 1, 0, CORNER);
  game_set_piece_orientation(g4, 1, 0, NORTH);
  game_set_piece_shape(g4, 1, 1, CORNER);
  game_set_piece_orientation(g4, 1, 1, WEST);
  if (!game_won(g4)) {
    game_delete(g4);
    return false;
  }
  game_delete(g4);

  // Avec le wrapping Une ligne remplie de segments est connectée
  game g5 = game_new_empty_ext(3, 4, true);
  assert(g5 != NULL);
  for (int i = 0; i < g5->nb_cols; i++) {
    game_set_piece_shape(g5, 0, i, SEGMENT);
    game_set_piece_orientation(g5, 0, i, EAST);
  }

  if (!game_won(g5)) {
    game_delete(g5);
    return false;
  }
  game_delete(g5);

  game g6 = game_new_empty_ext(3, 4, false);
  assert(g6 != NULL);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      game_set_piece_shape(g6, i, j, SEGMENT);
      game_set_piece_orientation(g6, i, j, EAST);
    }
  }
  if (game_won(g6)) {
    game_delete(g6);
    return false;
  }
  game_delete(g6);
  return true;
}

bool test_famseye_game_is_well_paired() {
  // Jeu par défaut contient des mismatch
  game g1 = game_default();
  assert(g1 != NULL);
  assert(game_is_well_paired(g1) == false);
  game_delete(g1);

  // Solution par défaut bien appariée
  game g2 = game_default_solution();
  assert(g2 != NULL);
  assert(game_is_well_paired(g2) == true);
  game_delete(g2);

  // Jeu avec un réseau et des cases vides bien apparié
  game g3 = game_new_empty();

  game_set_piece_shape(g3, 0, 0, CORNER);
  game_set_piece_orientation(g3, 0, 0, EAST);
  game_set_piece_shape(g3, 0, 1, CORNER);
  game_set_piece_orientation(g3, 0, 1, SOUTH);
  game_set_piece_shape(g3, 1, 0, CORNER);
  game_set_piece_orientation(g3, 1, 0, NORTH);
  game_set_piece_shape(g3, 1, 1, CORNER);
  game_set_piece_orientation(g3, 1, 1, WEST);

  assert(game_is_well_paired(g3) == true);
  game_delete(g3);

  // jeu vide bien apparié
  game g4 = game_new_empty();
  assert(g4 != NULL);
  assert(game_is_well_paired(g4) == true);
  game_delete(g4);

  shape shapes[] = {TEE, CORNER, SEGMENT, ENDPOINT};
  int num_shapes = 4;

  bool test_passed = true;

  for (int i = 0; i < num_shapes; i++) {
    for (int j = 0; j < num_shapes; j++) {
      for (direction dir1 = NORTH; dir1 <= WEST; dir1++) {
        for (direction dir2 = NORTH; dir2 <= WEST; dir2++) {
          game g = game_new_empty();
          assert(g != NULL);

          game_set_piece_shape(g, 0, 0, shapes[i]);
          game_set_piece_orientation(g, 0, 0, dir1);
          game_set_piece_shape(g, 0, 1, shapes[j]);
          game_set_piece_orientation(g, 0, 1, dir2);

          edge_status edge_status_result = game_check_edge(g, 0, 0, EAST);
          bool well_paired = game_is_well_paired(g);

          if ((edge_status_result == MISMATCH && well_paired == true) || (edge_status_result == MATCH && well_paired == false)) {
            test_passed = false;
            game_delete(g);
            break;
          }
        }
        if (!test_passed) {
          break;
        }
      }
      if (!test_passed) {
        break;
      }
    }
    if (!test_passed) {
      break;
    }
  }

  return test_passed;
}

// verifie qu'un jeu est le jeu vide
void assert_empty_game(game g) {
  assert(g != NULL);
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      assert(game_get_piece_shape(g, i, j) == EMPTY);
      assert(game_get_piece_orientation(g, i, j) == NORTH);
    }
  }
}

bool test_game_new_empty_ext() {
  game g = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false);
  assert_empty_game(g);
  game_delete(g);
  g = NULL;
  assert(g == NULL);
  return true;
}

// vérifie le résultat renvoyé par game_nb_rows
bool test_game_nb_rows() {
  game g1 = game_new_ext(6, 8, NULL, NULL, false);
  assert(game_nb_rows(g1) == 6);
  game_delete(g1);

  game g2 = game_new_ext(6, 6, NULL, NULL, true);
  assert(game_nb_rows(g2) == 6);
  game_delete(g2);

  game g3 = game_new_ext(1, 10, NULL, NULL, false);
  assert(game_nb_rows(g3) == 1);
  game_delete(g3);

  game g4 = game_new_ext(4, 1, NULL, NULL, false);
  assert(game_nb_rows(g4) == 4);
  game_delete(g4);
  return true;
}

bool test_game_new_ext() {
  shape shapes[] = {ENDPOINT, SEGMENT, TEE,    CORNER,   EMPTY,   SEGMENT, TEE,    CORNER,   ENDPOINT, SEGMENT, TEE,    CORNER, ENDPOINT,
                    SEGMENT,  TEE,     CORNER, ENDPOINT, SEGMENT, TEE,     CORNER, ENDPOINT, SEGMENT,  TEE,     CORNER, EMPTY};
  direction orientations[] = {NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST, NORTH,
                              EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH};

  game g = game_new_ext(5, 5, shapes, orientations, true);
  assert(g != NULL);
  // Verification de toutes les cases
  for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 5; y++) {
      int index = x * 5 + y;
      assert(game_get_piece_shape(g, x, y) == shapes[index]);
      assert(game_get_piece_orientation(g, x, y) == orientations[index]);
    }
  }
  game_delete(g);

  game g_null = game_new_ext(4, 5, NULL, NULL, true);
  assert(g_null != NULL);  // Là on verifie que le jeu est créé
  assert_empty_game(g_null);
  game_delete(g_null);
  // tests avec un jeu avec une seule entrée
  shape single_s[] = {
      TEE,   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  };

  direction single_orientation[] = {
      SOUTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
  };

  game g_single = game_new_ext(5, 5, single_s, single_orientation, true);
  assert(g_single != NULL);
  // Verification de toutes les cases
  for (int x = 0; x < DEFAULT_SIZE; x++) {
    for (int y = 0; y < DEFAULT_SIZE; y++) {
      int index = x * DEFAULT_SIZE + y;
      assert(game_get_piece_shape(g_single, x, y) == single_s[index]);
      assert(game_get_piece_orientation(g_single, x, y) == single_orientation[index]);
    }
  }
  game_delete(g_single);
  return true;
}

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (argc == 1) usage(argc, argv);

  bool ok = false;
  if (strcmp("game_play_move", argv[1]) == 0)
    ok = test_famseye_play_move();
  else if (strcmp("game_new_ext", argv[1]) == 0) {
    ok = test_game_new_ext();
  } else if (strcmp("game_nb_rows", argv[1]) == 0) {
    ok = test_game_nb_rows();
  } else if (strcmp("game_new_empty_ext", argv[1]) == 0) {
    ok = test_game_new_empty_ext();
  } else if (strcmp("get_piece_orientation", argv[1]) == 0) {
    ok = test_famseye_get_piece_orientation();
  } else if (strcmp("get_piece_shape", argv[1]) == 0) {
    ok = test_famseye_get_piece_shape();
  } else if (strcmp("reset_orientation", argv[1]) == 0) {
    ok = test_famseye_game_reset_orientation();
  } else if (strcmp("shuffle_orientation", argv[1]) == 0) {
    ok = test_famseye_game_shuffle_orientation();
  } else if (strcmp("game_won", argv[1]) == 0) {
    ok = test_famseye_game_won();
  } else if (strcmp("game_is_connected", argv[1]) == 0) {
    ok = test_famseye_game_is_connected();
  } else if (strcmp("dummy", argv[1]) == 0) {
    return test_famseye_dummy();
  } else if (strcmp("game_is_well_paired", argv[1]) == 0) {
    return test_famseye_game_is_well_paired();
  } else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  if (ok) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  }

  else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}
