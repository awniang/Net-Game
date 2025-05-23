#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

void assert_empty_game(game g) {
  assert(g != NULL);
  assert(DEFAULT_SIZE > 0);
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      assert(game_get_piece_shape(g, i, j) == EMPTY);
      assert(game_get_piece_orientation(g, i, j) == NORTH);
    }
  }
}
void test_dummy() {}

void test_game_new_empty() {
  game g = game_new_empty();
  assert_empty_game(g);
  game_delete(g);
  g = NULL;
  assert(g == NULL);
  printf("test_game_empty passed!\n");
}
void test_game_new() {
  assert(DEFAULT_SIZE > 0);
  shape shapes[] = {ENDPOINT, SEGMENT, TEE,    CORNER,   EMPTY,   SEGMENT, TEE,    CORNER,   ENDPOINT, SEGMENT, TEE,    CORNER, ENDPOINT,
                    SEGMENT,  TEE,     CORNER, ENDPOINT, SEGMENT, TEE,     CORNER, ENDPOINT, SEGMENT,  TEE,     CORNER, EMPTY};
  direction orientations[] = {NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST, NORTH,
                              EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH};

  game g = game_new(shapes, orientations);
  assert(g != NULL);
  // Verification de toutes les cases
  for (int x = 0; x < DEFAULT_SIZE; x++) {
    for (int y = 0; y < DEFAULT_SIZE; y++) {
      int index = x * DEFAULT_SIZE + y;
      assert(game_get_piece_shape(g, x, y) == shapes[index]);
      assert(game_get_piece_orientation(g, x, y) == orientations[index]);
    }
  }
  game_delete(g);

  game g_null = game_new(NULL, NULL);
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

  game g_single = game_new(single_s, single_orientation);
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

  printf("test_game_new passed!\n");
}

void test_game_copy() {
  shape shapes[] = {ENDPOINT, SEGMENT, TEE,    CORNER,   EMPTY,   SEGMENT, TEE,    CORNER,   ENDPOINT, SEGMENT, TEE,    CORNER, ENDPOINT,
                    SEGMENT,  TEE,     CORNER, ENDPOINT, SEGMENT, TEE,     CORNER, ENDPOINT, SEGMENT,  TEE,     CORNER, EMPTY};
  direction orientations[] = {NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST, NORTH,
                              EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH};
  game original_g = game_new(shapes, orientations);
  assert(original_g != NULL);

  game_play_move(original_g, 0, 0, -1);  // Exemple d'un coup joué
  game_play_move(original_g, 1, 1, -1);  // Un autre coup

  // Tester undo
  game_undo(original_g);  // Annuler le dernier mouvement
  game copied_after_undo = game_copy(original_g);
  // Vérifier l'état après undo
  assert(game_get_piece_shape(original_g, 0, 0) == game_get_piece_shape(copied_after_undo, 0, 0));
  assert(game_get_piece_orientation(original_g, 0, 0) == game_get_piece_orientation(copied_after_undo, 0, 0));

  // Tester redo
  game_redo(original_g);  // Rejouer le dernier coup annulé
  game copied_after_redo = game_copy(original_g);

  // Vérifier l'état après redo
  assert(game_get_piece_shape(original_g, 1, 1) == game_get_piece_shape(copied_after_redo, 1, 1));
  assert(game_get_piece_orientation(original_g, 1, 1) == game_get_piece_orientation(copied_after_redo, 1, 1));

  game_delete(copied_after_undo);
  game_delete(copied_after_redo);

  game copied_g = game_copy(original_g);
  assert(copied_g != NULL);
  uint nb_rows = game_nb_rows(original_g);
  uint nb_cols = game_nb_cols(original_g);
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      assert(game_get_piece_shape(original_g, i, j) == game_get_piece_shape(copied_g, i, j));
      assert(game_get_piece_orientation(original_g, i, j) == game_get_piece_orientation(copied_g, i, j));
    }
  }
  assert(game_nb_rows(original_g) == game_nb_rows(copied_g));
  assert(game_nb_cols(original_g) == game_nb_cols(copied_g));
  game_delete(original_g);
  game_delete(copied_g);

  // ON va ici tester la copie d'un jeu vide
  game empty_g = game_new_empty();
  assert(empty_g != NULL);
  game copied_empty_g = game_copy(empty_g);
  assert(copied_empty_g != NULL);

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      assert(game_get_piece_shape(empty_g, i, j) == game_get_piece_shape(copied_empty_g, i, j));
      assert(game_get_piece_orientation(empty_g, i, j) == game_get_piece_orientation(copied_empty_g, i, j));
    }
  }
  game_delete(empty_g);
  game_delete(copied_empty_g);

  // Test copie d'un jeu avec taille petite
  game small_g = game_new_ext(2, 2, shapes, orientations, false);
  game copied_small_g = game_copy(small_g);
  assert(game_nb_rows(small_g) == game_nb_rows(copied_small_g));
  assert(game_nb_cols(small_g) == game_nb_cols(copied_small_g));
  game_delete(small_g);
  game_delete(copied_small_g);

  game g_wrapping = game_new_ext(3, 3, shapes, orientations, true);  // Wrapping activé
  game copied_g_wrapping = game_copy(g_wrapping);
  assert(game_is_wrapping(g_wrapping) == game_is_wrapping(copied_g_wrapping));  // Vérification du wrapping
  game_delete(g_wrapping);
  game_delete(copied_g_wrapping);
  printf("test_game_copy passed!\n");
}
void test_game_equal() {
  shape shapes[] = {ENDPOINT, SEGMENT, TEE,    CORNER,   EMPTY,   SEGMENT, TEE,    CORNER,   ENDPOINT, SEGMENT, TEE,    CORNER, ENDPOINT,
                    SEGMENT,  TEE,     CORNER, ENDPOINT, SEGMENT, TEE,     CORNER, ENDPOINT, SEGMENT,  TEE,     CORNER, EMPTY};
  direction orientations[] = {NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST, NORTH,
                              EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH};
  game g1 = game_new(shapes, orientations);
  game g2 = game_copy(g1);

  assert(game_equal(g1, g2, true) == true);
  assert(game_equal(g1, g2, false) == true);

  direction new_orientations[] = {// on modifie juste une orientation
                                  NORTH, EAST,  SOUTH, EAST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST, NORTH,
                                  EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH, EAST,  SOUTH, WEST,  NORTH};
  game g3 = game_new(shapes, new_orientations);
  assert(game_equal(g1, g3, false) == false);
  assert(game_equal(g1, g3, true) == true);

  game g6 = game_new_ext(3, 3, shapes, orientations, false);
  game g7 = game_new_ext(4, 4, shapes, orientations, false);
  assert(game_equal(g6, g7, true) == false);
  assert(game_equal(g6, g7, false) == false);
  game_delete(g6);
  game_delete(g7);
  shape shapes_diff[] = {ENDPOINT, SEGMENT, TEE,    CORNER,   EMPTY,   SEGMENT, TEE,    CORNER,   SEGMENT, SEGMENT, TEE,    CORNER, ENDPOINT,
                         SEGMENT,  TEE,     CORNER, ENDPOINT, SEGMENT, TEE,     CORNER, ENDPOINT, SEGMENT, TEE,     CORNER, EMPTY};

  game g8 = game_new_ext(5, 5, shapes, orientations, false);
  game g9 = game_new_ext(5, 5, shapes_diff, orientations, false);
  assert(game_equal(g8, g9, true) == false);
  assert(game_equal(g8, g9, false) == false);
  game_delete(g8);
  game_delete(g9);

  shape empty_with_cross[DEFAULT_SIZE * DEFAULT_SIZE] = {CROSS};  // Jeu rempli uniquement de CROSS
  direction empty_orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {NORTH};
  game g16 = game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, empty_with_cross, empty_orientations, false);
  game g17 = game_copy(g16);

  assert(game_equal(g16, g17, true) == true);
  assert(game_equal(g16, g17, false) == true);

  game_delete(g16);
  game_delete(g17);

  direction random_orientations[] = {SOUTH, WEST, EAST,  NORTH, NORTH, SOUTH, WEST,  EAST, NORTH, SOUTH, EAST, WEST, SOUTH,
                                     EAST,  WEST, NORTH, WEST,  SOUTH, EAST,  NORTH, EAST, WEST,  SOUTH, WEST, NORTH};

  game g10 = game_new_ext(5, 5, shapes, orientations, false);
  game g11 = game_new_ext(5, 5, shapes, random_orientations, false);
  assert(game_equal(g10, g11, true) == true);    // Ignorer orientations -> Égalité
  assert(game_equal(g10, g11, false) == false);  // Prendre orientations en compte -> Différent
  game_delete(g10);
  game_delete(g11);

  // tests en cas de jeux vides
  game vide_g1 = game_new_empty();
  game vide_g2 = game_new_empty();
  assert(game_equal(vide_g1, vide_g2, false) == true);
  assert(game_equal(vide_g1, vide_g2, true) == true);
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(vide_g1);
  game_delete(vide_g2);
  printf("Test_game_equal passed!\n");
}
void test_game_delete() {
  game g = game_default();
  assert(g != NULL);
  game_delete(g);

  game g1 = game_new_ext(4, 5, NULL, NULL, false);
  assert(g1 != NULL);
  game_delete(g1);

  game g2 = game_new_ext(4, 5, NULL, NULL, true);
  assert(g2 != NULL);
  game_delete(g2);
  printf("test_game_delete passed!\n");
}
void test_game_set_piece_shape() {
  game g1 = game_default();
  assert(g1 != NULL);
  assert(2 < DEFAULT_SIZE && 3 < DEFAULT_SIZE);
  game_set_piece_shape(g1, 2, 3, TEE);
  assert(game_get_piece_shape(g1, 2, 3) == TEE);

  game_delete(g1);

  game g = game_new_ext(4, 5, NULL, NULL, false);
  assert(g != NULL);
  assert(2 < 4 && 3 < 5);
  game_set_piece_shape(g, 2, 3, TEE);
  assert(game_get_piece_shape(g, 2, 3) == TEE);

  game_delete(g);
  printf("test_game_set_piece_shape passed!\n");
}
void test_game_set_piece_orientation() {
  game g = game_new_ext(6, 8, NULL, NULL, false);
  assert(g != NULL);

  uint rows = game_nb_rows(g);
  uint cols = game_nb_cols(g);

  assert(2 < rows && 3 < cols);
  game_set_piece_orientation(g, 2, 3, EAST);
  assert(game_get_piece_orientation(g, 2, 3) == EAST);

  game_set_piece_orientation(g, 0, 0, SOUTH);
  assert(game_get_piece_orientation(g, 0, 0) == SOUTH);

  assert(rows <= game_nb_rows(g));
  assert(cols <= game_nb_cols(g));
  game_delete(g);

  game g1 = game_default();
  assert(g1 != NULL);
  assert(2 < DEFAULT_SIZE && 3 < DEFAULT_SIZE);
  game_set_piece_orientation(g1, 2, 3, EAST);
  assert(game_get_piece_orientation(g1, 2, 3) == EAST);

  game_set_piece_orientation(g1, 0, 0, SOUTH);
  assert(game_get_piece_orientation(g1, 0, 0) == SOUTH);

  game_delete(g1);
  printf("test_game_set_piece_orientation passed!\n");
}
void test_game_nb_cols() {
  game g1 = game_new_ext(6, 8, NULL, NULL, false);
  assert(game_nb_cols(g1) == 8);
  game_delete(g1);

  game g2 = game_new_ext(6, 6, NULL, NULL, true);
  assert(game_nb_cols(g2) == 6);
  game_delete(g2);

  game g3 = game_new_ext(1, 10, NULL, NULL, false);
  assert(game_nb_cols(g3) == 10);
  game_delete(g3);

  game g4 = game_new_ext(4, 1, NULL, NULL, false);
  assert(game_nb_cols(g4) == 1);
  game_delete(g4);
  printf("test_game_nb_cols passed!\n");
}
char shape_to_char(shape s) {
  switch (s) {
    case EMPTY:
      return 'E';
    case ENDPOINT:
      return 'N';
    case SEGMENT:
      return 'S';
    case CORNER:
      return 'C';
    case TEE:
      return 'T';
    case CROSS:
      return 'X';
    default:
      return '?';
  }
}

char direction_to_char(direction d) {
  switch (d) {
    case NORTH:
      return 'N';
    case EAST:
      return 'E';
    case SOUTH:
      return 'S';
    case WEST:
      return 'W';
    default:
      return '?';
  }
}

void test_game_load() {
  char* filename = "game_load.txt";
  FILE* file = fopen(filename, "w");
  assert(file != NULL);
  fprintf(file, "2 2 1 \n");
  fprintf(file, "CN NN \n");
  fprintf(file, "TS TW \n");
  fclose(file);
  shape shapes[] = {CORNER, ENDPOINT, TEE, TEE};
  direction dirs[] = {NORTH, NORTH, SOUTH, WEST};
  game g = game_new_ext(2, 2, shapes, dirs, true);
  assert(g != NULL);
  game loaded_game = game_load(filename);
  assert(loaded_game != NULL);
  assert(game_equal(g, loaded_game, false));
  game_delete(g);
  game_delete(loaded_game);
}

void test_game_save() {
  char* filename = "game_save.txt";
  shape shapes[] = {ENDPOINT, SEGMENT, CORNER, TEE, TEE, CORNER};
  direction dir[] = {NORTH, EAST, SOUTH, WEST, EAST, SOUTH};
  game g = game_new_ext(3, 2, shapes, dir, false);
  assert(g != NULL);
  game_save(g, filename);
  FILE* f = fopen(filename, "r");
  assert(f != NULL);
  int nb_rows, nb_cols, wrapping;
  assert(fscanf(f, "%d %d %d\n", &nb_rows, &nb_cols, &wrapping) == 3);
  assert(nb_rows == 3);
  assert(nb_cols == 2);
  assert(wrapping == 0);
  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      char shape_c, dir_c;
      assert(fscanf(f, " %c%c ", &shape_c, &dir_c) == 2);
      assert(shape_c == shape_to_char(shapes[i * nb_cols + j]));
      assert(dir_c == direction_to_char(dir[i * nb_cols + j]));
    }
  }
  fclose(f);
  game loaded_game = game_load(filename);
  assert(game_equal(g, loaded_game, false));

  game_delete(g);
  game_delete(loaded_game);

  char* filename2 = "game_save2.txt";
  shape shapes2[] = {CROSS, SEGMENT, ENDPOINT, TEE};
  direction dir2[] = {SOUTH, WEST, SOUTH, NORTH};
  game g2 = game_new_ext(2, 2, shapes2, dir2, true);
  assert(g2 != NULL);
  game_save(g2, filename2);
  FILE* f2 = fopen(filename2, "r");
  assert(f2 != NULL);
  char shape, direction;
  int count = 0;
  assert(fscanf(f2, "%d %d %d\n", &nb_rows, &nb_cols, &wrapping) == 3);
  assert(nb_rows == 2);
  assert(nb_cols == 2);
  assert(wrapping == 1);
  while (fscanf(f2, " %c%c", &shape, &direction) == 2) {
    count++;
  }
  assert(count == nb_rows * nb_cols);
  game_delete(g2);
  fclose(f2);
}

void test_game_is_wrapping() {
  game g1 = game_new_ext(4, 5, NULL, NULL, false);
  assert(g1 != NULL);
  assert(!game_is_wrapping(g1));
  assert(game_nb_rows(g1) == 4 && game_nb_cols(g1) == 5);
  game_delete(g1);

  game g2 = game_new_ext(4, 5, NULL, NULL, true);
  assert(g2 != NULL);
  assert(game_is_wrapping(g2));
  assert(game_nb_rows(g2) == 4 && game_nb_cols(g2) == 5);
  game_delete(g2);

  game g3 = game_new_ext(1, 1, NULL, NULL, true);
  assert(g3 != NULL);
  assert(game_is_wrapping(g3));
  assert(game_nb_rows(g3) == 1 && game_nb_cols(g3) == 1);
  game_delete(g3);

  game g4 = game_new_ext(1, 1, NULL, NULL, false);
  assert(g4 != NULL);
  assert(!game_is_wrapping(g4));
  assert(game_nb_rows(g4) == 1 && game_nb_cols(g4) == 1);
  game_delete(g4);
  printf("test_game_is_wrapping passed!\n");
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "dummy") == 0) {
    test_dummy();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_new_empty") == 0) {
    test_game_new_empty();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_new") == 0) {
    test_game_new();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_equal") == 0) {
    test_game_equal();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_delete") == 0) {
    test_game_delete();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_set_piece_shape") == 0) {
    test_game_set_piece_shape();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_copy") == 0) {
    test_game_copy();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_set_piece_orientation") == 0) {
    test_game_set_piece_orientation();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_nb_cols") == 0) {
    test_game_nb_cols();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_is_wrapping") == 0) {
    test_game_is_wrapping();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_load") == 0) {
    test_game_load();
    return EXIT_SUCCESS;
  } else if (strcmp(argv[1], "game_save") == 0) {
    test_game_save();
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}