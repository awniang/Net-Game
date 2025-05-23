#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

uint ROWS = 7;
uint COLS = 8;

shape SHAPES[] = {CORNER, TEE,   TEE,   TEE,   TEE,   TEE,   TEE,    CORNER, TEE,   CROSS, CROSS, CROSS, CROSS, CROSS,
                  CROSS,  TEE,   TEE,   CROSS, CROSS, CROSS, CROSS,  CROSS,  CROSS, TEE,   TEE,   CROSS, CROSS, CROSS,
                  CROSS,  CROSS, CROSS, TEE,   TEE,   CROSS, CROSS,  CROSS,  CROSS, CROSS, CROSS, TEE,   TEE,   CROSS,
                  CROSS,  CROSS, CROSS, CROSS, CROSS, TEE,   CORNER, TEE,    TEE,   TEE,   TEE,   TEE,   TEE,   CORNER};

direction ORIENTATIONS[] = {WEST,  SOUTH, SOUTH, SOUTH, SOUTH, SOUTH, SOUTH, SOUTH, EAST,  NORTH, NORTH, NORTH, NORTH, NORTH,
                            NORTH, WEST,  EAST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, WEST,  EAST,  NORTH, NORTH, NORTH,
                            NORTH, NORTH, NORTH, WEST,  EAST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, WEST,  EAST,  NORTH,
                            NORTH, NORTH, NORTH, NORTH, NORTH, WEST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, WEST};

int test_dummy() { return EXIT_SUCCESS; }

bool test_game_print() {
  game g = game_new_empty();
  assert(g != NULL);
  game_print(g);
  game_delete(g);

  game g_ext = game_new_empty_ext(ROWS, COLS, false);
  assert(g_ext != NULL && game_nb_rows(g_ext) <= 10 && game_nb_cols(g_ext) <= 10);
  game_print(g_ext);
  game_delete(g_ext);

  printf("test_game_print passed\n");
  return true;
}

bool test_game_default() {
  game g = game_default();
  assert(g != NULL);

  shape shapes[] = {CORNER,   ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,    TEE,     TEE,      TEE, TEE,      ENDPOINT, ENDPOINT, TEE,
                    ENDPOINT, SEGMENT,  ENDPOINT, TEE,    TEE,      CORNER, SEGMENT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT};

  direction orientations[] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST, EAST,  EAST, NORTH, WEST,
                              WEST, EAST,  SOUTH, SOUTH, NORTH, WEST,  NORTH, EAST,  WEST, SOUTH, EAST, SOUTH};

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      uint index = i * DEFAULT_SIZE + j;

      shape actual_shape = game_get_piece_shape(g, i, j);
      direction actual_orientation = game_get_piece_orientation(g, i, j);

      printf("Checking Position (%u, %u):\n", i, j);
      printf("Expected shape: %d, Actual shape: %d\n", shapes[index], actual_shape);
      printf("Expected orientation: %d, Actual orientation: %d\n", orientations[index], actual_orientation);

      assert(actual_shape == shapes[index] && "Shape does not match!");
      assert(actual_orientation == orientations[index] && "Orientation does not match!");
    }
  }

  game_delete(g);
  printf("test_game_default passed!\n");
  return true;
}

bool test_game_default_solution() {
  game g = game_default_solution();
  assert(g != NULL);
  assert(game_won(g) == true);
  game_delete(g);
  printf("test_game_default_solution passed.\n");
  return true;
}

bool test_game_get_ajacent_square() {
  game g = game_default();
  assert(g != NULL);
  uint i_next, j_next;

  int DIR2OFFSET[][2] = {
      [NORTH] = {-1, 0},
      [EAST] = {0, 1},
      [SOUTH] = {1, 0},
      [WEST] = {0, -1},
  };
  // check case HAVE adjacent squqre
  for (int d = 0; d < NB_DIRS; d++) {
    assert(game_get_ajacent_square(g, 1, 1, d, &i_next, &j_next));
    if (!(i_next == 1 + DIR2OFFSET[d][0]) || !(j_next == 1 + DIR2OFFSET[d][1])) return false;
  };

  // check case DON'T HAVE adjacent squqre
  int check_case[][2] = {{0, 0}, {0, 4}, {4, 4}, {4, 0}};
  for (int d = 0; d < NB_DIRS; d++) {
    if (game_get_ajacent_square(g, check_case[d][0], check_case[d][1], d, &i_next, &j_next)) return false;
  }
  game_delete(g);

  printf("test_game_get_ajacent_square passed!\n");
  return true;
}

bool test_game_get_ajacent_square_ext() {
  // ext no wrapping
  game g_ext = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, false);
  assert(g_ext != NULL);
  uint i_next = 0, j_next = 0;

  assert(!game_get_ajacent_square(g_ext, 0, 0, NORTH, &i_next, &j_next));
  assert(!game_get_ajacent_square(g_ext, 0, 0, WEST, &i_next, &j_next));

  assert(game_get_ajacent_square(g_ext, 1, 1, EAST, &i_next, &j_next));
  assert(i_next == 1 && j_next == 2);
  printf("i_next should be 1 and got %d,j_next should be 2 and got %d\n", i_next, j_next);
  assert(game_get_ajacent_square(g_ext, 1, 1, SOUTH, &i_next, &j_next));
  assert(i_next == 2 && j_next == 1);

  assert(!game_get_ajacent_square(g_ext, 6, 0, SOUTH, &i_next, &j_next));
  assert(!game_get_ajacent_square(g_ext, 6, 0, WEST, &i_next, &j_next));

  assert(!game_get_ajacent_square(g_ext, 0, 7, NORTH, &i_next, &j_next));
  assert(!game_get_ajacent_square(g_ext, 0, 7, EAST, &i_next, &j_next));

  assert(!game_get_ajacent_square(g_ext, 6, 7, SOUTH, &i_next, &j_next));
  assert(!game_get_ajacent_square(g_ext, 6, 7, EAST, &i_next, &j_next));

  game_delete(g_ext);

  // ext wraping
  game g_ext_w = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, true);
  assert(g_ext_w != NULL);

  assert(game_get_ajacent_square(g_ext_w, 0, 0, NORTH, &i_next, &j_next));
  assert(i_next == 6 && j_next == 0);
  assert(game_get_ajacent_square(g_ext_w, 0, 0, WEST, &i_next, &j_next));
  assert(i_next == 0 && j_next == 7);

  assert(game_get_ajacent_square(g_ext_w, 1, 1, EAST, &i_next, &j_next));
  assert(i_next == 1 && j_next == 2);
  assert(game_get_ajacent_square(g_ext_w, 1, 1, SOUTH, &i_next, &j_next));
  assert(i_next == 2 && j_next == 1);

  assert(game_get_ajacent_square(g_ext_w, 6, 0, SOUTH, &i_next, &j_next));
  assert(i_next == 0 && j_next == 0);
  assert(game_get_ajacent_square(g_ext_w, 6, 0, WEST, &i_next, &j_next));
  assert(i_next == 6 && j_next == 7);

  assert(game_get_ajacent_square(g_ext_w, 0, 7, NORTH, &i_next, &j_next));
  assert(i_next == 6 && j_next == 7);
  assert(game_get_ajacent_square(g_ext_w, 0, 7, EAST, &i_next, &j_next));
  assert(i_next == 0 && j_next == 0);

  assert(game_get_ajacent_square(g_ext_w, 6, 7, SOUTH, &i_next, &j_next));
  assert(i_next == 0 && j_next == 7);
  assert(game_get_ajacent_square(g_ext_w, 6, 7, EAST, &i_next, &j_next));
  assert(i_next == 6 && j_next == 0);

  game_delete(g_ext_w);

  printf("test_game_get_ajacent_square_ext passed!\n");
  return true;
}

bool test_game_has_half_edge() {
  game g = game_default();
  assert(g != NULL);
  printf("game_default : \n");
  game_print(g);

  game_set_piece_shape(g, 0, 0, ENDPOINT);
  game_set_piece_orientation(g, 0, 0, NORTH);

  game_set_piece_shape(g, 0, 1, SEGMENT);
  game_set_piece_orientation(g, 0, 1, EAST);

  game_set_piece_shape(g, 1, 0, CORNER);
  game_set_piece_orientation(g, 1, 0, SOUTH);

  game_set_piece_shape(g, 1, 1, TEE);
  game_set_piece_orientation(g, 1, 1, WEST);

  printf("Checking half edges for set pieces:\n");
  printf("Piece (0,0) North: %d (expected: true)\n", game_has_half_edge(g, 0, 0, NORTH));
  assert(game_has_half_edge(g, 0, 0, NORTH) == true);

  printf("Piece (0,1) East: %d (expected: true)\n", game_has_half_edge(g, 0, 1, EAST));
  assert(game_has_half_edge(g, 0, 1, EAST) == true);

  printf("Piece (1,0) South: %d (expected: true)\n", game_has_half_edge(g, 1, 0, SOUTH));
  assert(game_has_half_edge(g, 1, 0, SOUTH) == true);

  printf("Piece (1,1) West: %d (expected: true)\n", game_has_half_edge(g, 1, 1, WEST));
  assert(game_has_half_edge(g, 1, 1, WEST) == true);

  printf("Checking edges without half edges:\n");
  printf("Piece (0,0) East: %d (expected: false)\n", game_has_half_edge(g, 0, 0, EAST));
  assert(game_has_half_edge(g, 0, 0, EAST) == false);

  printf("Piece (0,1) North: %d (expected: false)\n", game_has_half_edge(g, 0, 1, NORTH));
  assert(game_has_half_edge(g, 0, 1, NORTH) == false);

  printf("Piece (1,0) East: %d (expected: false)\n", game_has_half_edge(g, 1, 0, EAST));
  assert(game_has_half_edge(g, 1, 0, EAST) == false);

  printf("Piece (1,1) North: %d (expected: true)\n", game_has_half_edge(g, 1, 1, NORTH));
  assert(game_has_half_edge(g, 1, 1, NORTH) == true);

  printf("test_game_has_half_edge passed!\n");
  game_delete(g);
  return true;
}

bool test_game_has_half_edge_ext() {
  // ext no wrapping
  game g_ext = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, false);
  assert(g_ext != NULL);
  game_set_piece_shape(g_ext, 0, 0, ENDPOINT);
  game_set_piece_orientation(g_ext, 0, 0, NORTH);

  game_set_piece_shape(g_ext, 0, 1, SEGMENT);
  game_set_piece_orientation(g_ext, 0, 1, EAST);

  game_set_piece_shape(g_ext, 1, 0, CORNER);
  game_set_piece_orientation(g_ext, 1, 0, SOUTH);

  game_set_piece_shape(g_ext, 1, 1, TEE);
  game_set_piece_orientation(g_ext, 1, 1, WEST);

  game_set_piece_shape(g_ext, 2, 0, CROSS);
  game_set_piece_orientation(g_ext, 2, 0, WEST);

  // endpoint north expected true : north
  assert(game_has_half_edge(g_ext, 0, 0, NORTH) == true);
  assert(game_has_half_edge(g_ext, 0, 0, EAST) == false);
  assert(game_has_half_edge(g_ext, 0, 0, WEST) == false);
  assert(game_has_half_edge(g_ext, 0, 0, SOUTH) == false);

  // segment east expected true : east west
  assert(game_has_half_edge(g_ext, 0, 1, EAST) == true);
  assert(game_has_half_edge(g_ext, 0, 1, WEST) == true);
  assert(game_has_half_edge(g_ext, 0, 1, NORTH) == false);
  assert(game_has_half_edge(g_ext, 0, 1, SOUTH) == false);

  // corner south expected true : south west
  assert(game_has_half_edge(g_ext, 1, 0, SOUTH) == true);
  assert(game_has_half_edge(g_ext, 1, 0, WEST) == true);
  assert(game_has_half_edge(g_ext, 1, 0, EAST) == false);
  assert(game_has_half_edge(g_ext, 1, 0, NORTH) == false);

  // tee west expected true : west north south
  assert(game_has_half_edge(g_ext, 1, 1, WEST) == true);
  assert(game_has_half_edge(g_ext, 1, 1, NORTH) == true);
  assert(game_has_half_edge(g_ext, 1, 1, SOUTH) == true);
  assert(game_has_half_edge(g_ext, 1, 1, EAST) == false);

  // cross west expected true : west north south east
  assert(game_has_half_edge(g_ext, 2, 0, NORTH) == true);
  assert(game_has_half_edge(g_ext, 2, 0, SOUTH) == true);
  assert(game_has_half_edge(g_ext, 2, 0, EAST) == true);
  assert(game_has_half_edge(g_ext, 2, 0, WEST) == true);

  /// ext wraping
  game g_ext_w = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, true);
  assert(g_ext_w != NULL);
  game_set_piece_shape(g_ext_w, 0, 0, ENDPOINT);
  game_set_piece_orientation(g_ext_w, 0, 0, NORTH);

  game_set_piece_shape(g_ext_w, 6, 0, SEGMENT);
  game_set_piece_orientation(g_ext_w, 6, 0, EAST);

  game_set_piece_shape(g_ext_w, 6, 7, CORNER);
  game_set_piece_orientation(g_ext_w, 6, 7, SOUTH);

  game_set_piece_shape(g_ext_w, 0, 7, TEE);
  game_set_piece_orientation(g_ext_w, 0, 7, WEST);

  game_set_piece_shape(g_ext_w, 2, 0, CROSS);
  game_set_piece_orientation(g_ext_w, 2, 0, WEST);

  // endpoint north expected true : north
  assert(game_has_half_edge(g_ext_w, 0, 0, NORTH) == true);
  assert(game_has_half_edge(g_ext_w, 0, 0, SOUTH) == false);
  assert(game_has_half_edge(g_ext_w, 0, 0, EAST) == false);
  assert(game_has_half_edge(g_ext_w, 0, 0, WEST) == false);

  // segment east expected true : east west
  assert(game_has_half_edge(g_ext_w, 6, 0, EAST) == true);
  assert(game_has_half_edge(g_ext_w, 6, 0, SOUTH) == false);
  assert(game_has_half_edge(g_ext_w, 6, 0, NORTH) == false);
  assert(game_has_half_edge(g_ext_w, 6, 0, WEST) == true);

  // corner south expected true : south west
  assert(game_has_half_edge(g_ext_w, 6, 7, SOUTH) == true);
  assert(game_has_half_edge(g_ext_w, 6, 7, WEST) == true);
  assert(game_has_half_edge(g_ext_w, 6, 7, EAST) == false);
  assert(game_has_half_edge(g_ext_w, 6, 7, NORTH) == false);

  // tee west expected true : west north south
  assert(game_has_half_edge(g_ext_w, 0, 7, WEST) == true);
  assert(game_has_half_edge(g_ext_w, 0, 7, NORTH) == true);
  assert(game_has_half_edge(g_ext_w, 0, 7, SOUTH) == true);
  assert(game_has_half_edge(g_ext_w, 0, 7, EAST) == false);

  // cross west expected true : west north south east
  assert(game_has_half_edge(g_ext_w, 2, 0, NORTH) == true);
  assert(game_has_half_edge(g_ext_w, 2, 0, SOUTH) == true);
  assert(game_has_half_edge(g_ext_w, 2, 0, EAST) == true);
  assert(game_has_half_edge(g_ext_w, 2, 0, WEST) == true);

  game_delete(g_ext);
  game_delete(g_ext_w);
  printf("test_game_has_half_edge_ext passed!\n");
  return true;
}

bool test_game_check_edge() {
  game g = game_default();
  assert(g != NULL);

  game_set_piece_shape(g, 0, 0, ENDPOINT);
  game_set_piece_orientation(g, 0, 0, EAST);

  game_set_piece_shape(g, 0, 1, ENDPOINT);
  game_set_piece_orientation(g, 0, 1, WEST);

  assert(game_check_edge(g, 0, 0, EAST) == MATCH);

  assert(game_check_edge(g, 0, 1, WEST) == MATCH);

  game_set_piece_shape(g, 1, 0, SEGMENT);
  game_set_piece_orientation(g, 1, 0, NORTH);

  game_set_piece_shape(g, 1, 1, CORNER);
  game_set_piece_orientation(g, 1, 1, WEST);

  assert(game_check_edge(g, 1, 0, EAST) == MISMATCH);

  assert(game_check_edge(g, 1, 1, WEST) == MISMATCH);

  game_set_piece_shape(g, 2, 0, EMPTY);

  assert(game_check_edge(g, 2, 0, EAST) == NOEDGE);

  assert(game_check_edge(g, 0, 0, NORTH) == NOEDGE);
  assert(game_check_edge(g, 0, 0, WEST) == NOEDGE);

  game_delete(g);
  printf("test_game_check_edge passed!\n");

  return true;
}

bool test_game_check_edge_ext() {
  // ext no wrapping
  game g_ext = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, false);
  assert(g_ext != NULL);

  edge_status status = game_check_edge(g_ext, 0, 0, EAST);
  printf("status: %d\n", status);
  assert(status == MISMATCH);

  status = game_check_edge(g_ext, 1, 1, EAST);
  assert(status == MATCH);

  game_set_piece_shape(g_ext, 2, 5, SEGMENT);
  game_set_piece_orientation(g_ext, 2, 5, WEST);
  status = game_check_edge(g_ext, 1, 5, SOUTH);
  assert(status == MISMATCH);

  game_set_piece_orientation(g_ext, 6, 0, WEST);
  game_set_piece_shape(g_ext, 6, 1, CORNER);
  game_set_piece_orientation(g_ext, 6, 1, EAST);
  status = game_check_edge(g_ext, 6, 0, EAST);
  assert(status == NOEDGE);

  game g2_ext = game_new_empty_ext(ROWS, COLS, false);
  assert(g2_ext != NULL);

  game_set_piece_shape(g2_ext, 0, 0, ENDPOINT);
  game_set_piece_orientation(g2_ext, 0, 0, EAST);
  game_set_piece_shape(g2_ext, 0, 1, ENDPOINT);
  game_set_piece_orientation(g2_ext, 0, 1, WEST);
  assert(game_check_edge(g2_ext, 0, 0, EAST) == MATCH);
  assert(game_check_edge(g2_ext, 0, 1, WEST) == MATCH);

  game_set_piece_shape(g2_ext, 1, 0, SEGMENT);
  game_set_piece_orientation(g2_ext, 1, 0, NORTH);
  game_set_piece_shape(g2_ext, 1, 1, CORNER);
  game_set_piece_orientation(g2_ext, 1, 1, WEST);
  assert(game_check_edge(g2_ext, 1, 0, EAST) == MISMATCH);
  assert(game_check_edge(g2_ext, 1, 1, WEST) == MISMATCH);
  game_set_piece_shape(g2_ext, 2, 0, EMPTY);
  assert(game_check_edge(g2_ext, 2, 0, EAST) == NOEDGE);
  assert(game_check_edge(g2_ext, 0, 0, NORTH) == NOEDGE);
  assert(game_check_edge(g2_ext, 0, 0, WEST) == NOEDGE);

  // ext wrapping
  shape s[] = {TEE,      SEGMENT, EMPTY, ENDPOINT, CORNER,   CORNER, EMPTY, EMPTY,   EMPTY,    EMPTY,
               ENDPOINT, EMPTY,   EMPTY, EMPTY,    ENDPOINT, TEE,    CROSS, SEGMENT, ENDPOINT, CROSS};
  direction o[] = {SOUTH, NORTH, NORTH, NORTH, EAST, WEST, NORTH, NORTH, NORTH, NORTH,
                   EAST,  NORTH, NORTH, NORTH, EAST, WEST, EAST,  EAST,  SOUTH, SOUTH};
  game g_ext_w = game_new_ext(4, 5, s, o, true);
  assert(g_ext_w != NULL);

  status = game_check_edge(g_ext_w, 0, 0, EAST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 0, 0, WEST);
  assert(status == MATCH);
  status = game_check_edge(g_ext_w, 0, 0, NORTH);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 0, 0, SOUTH);
  assert(status == MATCH);

  status = game_check_edge(g_ext_w, 0, 1, SOUTH);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 0, 1, EAST);
  assert(status == NOEDGE);
  status = game_check_edge(g_ext_w, 0, 1, WEST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 0, 1, NORTH);
  assert(status == MATCH);

  status = game_check_edge(g_ext_w, 1, 0, SOUTH);
  assert(status == NOEDGE);
  status = game_check_edge(g_ext_w, 1, 0, EAST);
  assert(status == NOEDGE);
  status = game_check_edge(g_ext_w, 1, 0, WEST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 1, 0, NORTH);
  assert(status == MATCH);

  status = game_check_edge(g_ext_w, 2, 0, SOUTH);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 2, 0, EAST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 2, 0, WEST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 2, 0, NORTH);
  assert(status == NOEDGE);

  status = game_check_edge(g_ext_w, 3, 0, SOUTH);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 3, 0, EAST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 3, 0, WEST);
  assert(status == MATCH);
  status = game_check_edge(g_ext_w, 3, 0, NORTH);
  assert(status == MISMATCH);

  status = game_check_edge(g_ext_w, 3, 1, SOUTH);
  assert(status == MATCH);
  status = game_check_edge(g_ext_w, 3, 1, EAST);
  assert(status == MATCH);
  status = game_check_edge(g_ext_w, 3, 1, WEST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 3, 1, NORTH);
  assert(status == MISMATCH);

  status = game_check_edge(g_ext_w, 3, 2, SOUTH);
  assert(status == NOEDGE);
  status = game_check_edge(g_ext_w, 3, 2, EAST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 3, 2, WEST);
  assert(status == MATCH);
  status = game_check_edge(g_ext_w, 3, 2, NORTH);
  assert(status == NOEDGE);

  status = game_check_edge(g_ext_w, 0, 3, SOUTH);
  assert(status == NOEDGE);
  status = game_check_edge(g_ext_w, 0, 3, EAST);
  assert(status == NOEDGE);
  status = game_check_edge(g_ext_w, 0, 3, WEST);
  assert(status == NOEDGE);
  status = game_check_edge(g_ext_w, 0, 3, NORTH);
  assert(status == MATCH);

  status = game_check_edge(g_ext_w, 0, 4, SOUTH);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 0, 4, EAST);
  assert(status == MATCH);
  status = game_check_edge(g_ext_w, 0, 4, WEST);
  assert(status == NOEDGE);
  status = game_check_edge(g_ext_w, 0, 4, NORTH);
  assert(status == MISMATCH);

  status = game_check_edge(g_ext_w, 3, 3, SOUTH);
  assert(status == MATCH);
  status = game_check_edge(g_ext_w, 3, 3, EAST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 3, 3, WEST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 3, 3, NORTH);
  assert(status == NOEDGE);

  status = game_check_edge(g_ext_w, 3, 4, SOUTH);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 3, 4, EAST);
  assert(status == MATCH);
  status = game_check_edge(g_ext_w, 3, 4, WEST);
  assert(status == MISMATCH);
  status = game_check_edge(g_ext_w, 3, 4, NORTH);
  assert(status == MISMATCH);

  game_delete(g_ext);
  game_delete(g2_ext);
  game_delete(g_ext_w);
  printf("test_game_check_edge_ext passed!\n");

  return true;
}

bool test_game_is_well_paired() {
  // 1.game not touched so not paired
  game g1 = game_default();
  assert(g1 != NULL);
  assert(game_is_well_paired(g1) == false);
  game_delete(g1);

  // 2.right game solution is paired
  game g2 = game_default_solution();
  assert(g2 != NULL);
  assert(game_is_well_paired(g2) == true);
  game_delete(g2);

  // 3.mismatched edge so is not paired
  game g3 = game_new_empty();
  assert(g3 != NULL);
  game_set_piece_shape(g3, 0, 0, ENDPOINT);
  game_set_piece_shape(g3, 0, 1, SEGMENT);
  game_set_piece_orientation(g3, 0, 0, EAST);
  game_set_piece_orientation(g3, 0, 1, EAST);
  assert(game_is_well_paired(g3) == false);
  game_delete(g3);

  // 4.empty game treated as paired
  game g4 = game_new_empty();
  assert(g4 != NULL);
  assert(game_is_well_paired(g4) == true);
  game_delete(g4);

  // 5.Only one whole column of connected elements / The effect of the “EMPTY” cell
  game g5 = game_new_empty();
  assert(g5 != NULL);
  game_set_piece_shape(g5, 0, 0, ENDPOINT);
  game_set_piece_orientation(g5, 0, 0, SOUTH);
  game_set_piece_shape(g5, 1, 0, SEGMENT);
  game_set_piece_shape(g5, 2, 0, SEGMENT);
  game_set_piece_shape(g5, 3, 0, SEGMENT);
  game_set_piece_shape(g5, 4, 0, ENDPOINT);
  game_set_piece_orientation(g5, 4, 0, NORTH);
  game_print(g5);
  assert(game_is_well_paired(g5) == true);

  // The connection is not affected even if an empty shape is added in other places.
  game_set_piece_shape(g5, 2, 2, EMPTY);
  assert(game_is_well_paired(g5) == true);

  /// Replacing one of the cases with a TEE will not work
  game_set_piece_shape(g5, 1, 0, TEE);
  game_set_piece_orientation(g5, 1, 0, EAST);
  assert(game_is_well_paired(g5) == false);

  ////Test if a random shape is added at another location works
  game_set_piece_shape(g5, 1, 0, SEGMENT);  // Modify it so that it connects correctly.
  game_set_piece_orientation(g5, 1, 0, NORTH);
  game_set_piece_shape(g5, 4, 4, ENDPOINT);  // add one for testing
  assert(game_is_well_paired(g5) == false);

  printf("g5 test passe\n");
  game_delete(g5);

  // 6. Only one complete connection line
  game g6 = game_new_empty();
  assert(g6 != NULL);
  game_set_piece_shape(g6, 0, 0, ENDPOINT);
  game_set_piece_orientation(g6, 0, 0, EAST);
  game_set_piece_shape(g6, 0, 1, SEGMENT);
  game_set_piece_shape(g6, 0, 2, SEGMENT);
  game_set_piece_shape(g6, 0, 3, SEGMENT);
  game_set_piece_orientation(g6, 0, 1, EAST);
  game_set_piece_orientation(g6, 0, 2, WEST);
  game_set_piece_orientation(g6, 0, 3, WEST);
  game_set_piece_shape(g6, 0, 4, ENDPOINT);
  game_set_piece_orientation(g6, 0, 4, WEST);
  game_print(g6);
  assert(game_is_well_paired(g6) == true);

  /// Replacing one of the cases with a TEE will not work
  game_set_piece_shape(g6, 0, 1, TEE);
  game_set_piece_orientation(g6, 0, 1, NORTH);
  assert(game_is_well_paired(g6) == false);

  //// Test if a random shape is added at another location works
  game_set_piece_shape(g6, 0, 1, SEGMENT);
  game_set_piece_orientation(g6, 0, 1, EAST);
  game_set_piece_shape(g6, 3, 3, SEGMENT);
  assert(game_is_well_paired(g6) == false);

  printf("g6 test passe\n");
  game_delete(g6);

  // 7. Separate connected blocks
  game g7 = game_new_empty();
  /// Test with one connected block first
  game_set_piece_shape(g7, 0, 0, CORNER);
  game_set_piece_shape(g7, 0, 1, SEGMENT);
  game_set_piece_shape(g7, 0, 2, CORNER);
  game_set_piece_shape(g7, 1, 0, SEGMENT);
  game_set_piece_shape(g7, 1, 2, SEGMENT);
  game_set_piece_shape(g7, 2, 0, CORNER);
  game_set_piece_shape(g7, 2, 1, SEGMENT);
  game_set_piece_shape(g7, 2, 2, CORNER);
  game_set_piece_orientation(g7, 0, 0, EAST);
  game_set_piece_orientation(g7, 0, 2, SOUTH);
  game_set_piece_orientation(g7, 2, 0, NORTH);
  game_set_piece_orientation(g7, 2, 2, WEST);

  game_set_piece_orientation(g7, 0, 1, WEST);
  game_set_piece_orientation(g7, 1, 0, NORTH);  // north south have no différence
  game_set_piece_orientation(g7, 2, 1, EAST);   // west east have no différence
  game_set_piece_orientation(g7, 1, 2, SOUTH);
  game_print(g7);
  assert(game_is_well_paired(g7) == true);
  game_set_piece_shape(g7, 1, 1,
                       SEGMENT);  // Bloc complet connecté avec « segment » au milieu du bloc non lié
  game_print(g7);
  assert(game_is_well_paired(g7) == false);
  game_set_piece_shape(g7, 1, 1, EMPTY);
  game_set_piece_shape(g7, 3, 3,
                       ENDPOINT);  // Un bloc connecté complet avec un shape aléatoire en extérieur de bloc
  assert(game_is_well_paired(g7) == false);
  game_set_piece_shape(g7, 3, 3, EMPTY);

  /// The other connected block
  game_set_piece_shape(g7, 3, 2, CORNER);
  game_set_piece_shape(g7, 3, 3, CORNER);
  game_set_piece_shape(g7, 4, 2, CORNER);
  game_set_piece_shape(g7, 4, 3, CORNER);
  game_set_piece_orientation(g7, 3, 2, EAST);
  game_set_piece_orientation(g7, 3, 3, SOUTH);
  game_set_piece_orientation(g7, 4, 2, NORTH);
  game_set_piece_orientation(g7, 4, 3, WEST);
  game_print(g7);
  assert(game_is_well_paired(g7) == true);

  game_delete(g7);

  // 8.  Closed-loop trajectory detection using TEE shape instead of CORNER
  game g8 = game_new_empty();
  assert(g8 != NULL);

  game_set_piece_shape(g8, 0, 0, TEE);
  game_set_piece_orientation(g8, 0, 0, SOUTH);

  game_set_piece_shape(g8, 1, 0, SEGMENT);
  game_set_piece_orientation(g8, 1, 0, SOUTH);

  game_set_piece_shape(g8, 2, 0, TEE);
  game_set_piece_orientation(g8, 2, 0, EAST);

  game_set_piece_shape(g8, 2, 1, SEGMENT);
  game_set_piece_orientation(g8, 2, 1, EAST);

  game_set_piece_shape(g8, 2, 2, TEE);
  game_set_piece_orientation(g8, 2, 2, NORTH);

  game_set_piece_shape(g8, 1, 2, SEGMENT);
  game_set_piece_orientation(g8, 1, 2, NORTH);

  game_set_piece_shape(g8, 0, 2, TEE);
  game_set_piece_orientation(g8, 0, 2, WEST);

  game_set_piece_shape(g8, 0, 1, SEGMENT);
  game_set_piece_orientation(g8, 0, 1, WEST);

  game_print(g8);
  assert(game_is_well_paired(g8) == false);

  game_delete(g8);

  // 9.  Check the connection of shapes in different directions in the four corners
  game g9 = game_new_empty();
  assert(g9 != NULL);
  direction d[] = {NORTH, SOUTH, WEST, EAST};
  shape s[] = {CORNER, TEE, SEGMENT, ENDPOINT};
  for (int i = 0; i < NB_DIRS; i++) {
    for (int j = 0; j < 4 - 1; j++) {
      game_set_piece_shape(g9, 0, 0, s[j]);
      game_set_piece_orientation(g9, 0, 0, d[i]);
      assert(game_is_well_paired(g9) == false);
      game_set_piece_shape(g9, 0, 0, EMPTY);

      game_set_piece_shape(g9, DEFAULT_SIZE - 1, 0, s[j]);
      game_set_piece_orientation(g9, DEFAULT_SIZE - 1, 0, d[i]);
      assert(game_is_well_paired(g9) == false);
      game_set_piece_shape(g9, DEFAULT_SIZE - 1, 0, EMPTY);

      game_set_piece_shape(g9, 0, DEFAULT_SIZE - 1, s[j]);
      game_set_piece_orientation(g9, 0, DEFAULT_SIZE - 1, d[i]);
      assert(game_is_well_paired(g9) == false);
      game_set_piece_shape(g9, 0, DEFAULT_SIZE - 1, EMPTY);

      game_set_piece_shape(g9, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, s[j]);
      game_set_piece_orientation(g9, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, d[i]);
      assert(game_is_well_paired(g9) == false);
      game_set_piece_shape(g9, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, EMPTY);
    }
  }

  game_delete(g9);

  // 10. Check for misaligned connections ( >-  empty  -< )
  game g10 = game_new_empty();
  game_set_piece_shape(g10, 0, 0, ENDPOINT);
  game_set_piece_orientation(g10, 0, 0, EAST);
  game_set_piece_shape(g10, 0, 2, ENDPOINT);
  game_set_piece_orientation(g10, 0, 2, WEST);
  game_print(g10);
  assert(game_is_well_paired(g10) == false);
  game_delete(g10);

  // 11. Check the CORNER's connection to wall
  game g11 = game_new_empty();
  game_set_piece_shape(g11, 0, 0, CORNER);
  game_set_piece_orientation(g11, 0, 0, SOUTH);
  game_set_piece_shape(g11, 1, 0, CORNER);
  game_set_piece_orientation(g11, 1, 0, WEST);
  game_set_piece_shape(g11, 0, DEFAULT_SIZE - 1, CORNER);
  game_set_piece_orientation(g11, 0, DEFAULT_SIZE - 1, EAST);
  game_set_piece_shape(g11, 1, DEFAULT_SIZE - 1, CORNER);
  game_set_piece_orientation(g11, 1, DEFAULT_SIZE - 1, NORTH);
  game_print(g11);
  assert(game_is_well_paired(g11) == false);
  game_delete(g11);

  printf("All test cases for test_game_is_well_paired passed!\n");
  return true;
}

bool test_game_is_well_paired_ext() {
  /// test for ext
  // 1.game not paired bc (0,0) have orientation west
  game g1_ext = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, false);
  assert(g1_ext != NULL);
  assert(game_is_well_paired(g1_ext) == false);
  printf("g1_ext pass\n");
  game_delete(g1_ext);

  // 2.right game solution is paired
  game g2_ext = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, false);
  assert(g2_ext != NULL);
  game_set_piece_orientation(g2_ext, 0, 0, EAST);
  assert(game_is_well_paired(g2_ext) == true);
  printf("g2_ext pass\n");
  game_delete(g2_ext);

  // 3.mismatched edge so is not paired
  game g3_ext = game_new_empty_ext(ROWS, COLS, false);
  assert(g3_ext != NULL);
  game_set_piece_shape(g3_ext, 0, 0, ENDPOINT);
  game_set_piece_shape(g3_ext, 0, 1, SEGMENT);
  game_set_piece_orientation(g3_ext, 0, 0, EAST);
  game_set_piece_orientation(g3_ext, 0, 1, EAST);
  assert(game_is_well_paired(g3_ext) == false);
  printf("g3_ext pass\n");
  game_delete(g3_ext);

  // 4.empty game treated as paired
  game g4_ext = game_new_empty_ext(ROWS, COLS, false);
  assert(g4_ext != NULL);
  assert(game_is_well_paired(g4_ext) == true);
  printf("g4_ext pass\n");
  game_delete(g4_ext);

  // 5.Only one whole column of connected elements / The effect of the “EMPTY”
  // cell
  game g5_ext = game_new_empty_ext(ROWS, COLS, false);
  assert(g5_ext != NULL);
  game_set_piece_shape(g5_ext, 0, 0, ENDPOINT);
  game_set_piece_orientation(g5_ext, 0, 0, SOUTH);
  game_set_piece_shape(g5_ext, 1, 0, SEGMENT);
  game_set_piece_shape(g5_ext, 2, 0, SEGMENT);
  game_set_piece_shape(g5_ext, 3, 0, SEGMENT);
  game_set_piece_shape(g5_ext, 4, 0, SEGMENT);
  game_set_piece_shape(g5_ext, 5, 0, SEGMENT);
  game_set_piece_shape(g5_ext, 6, 0, ENDPOINT);
  game_set_piece_orientation(g5_ext, 6, 0, NORTH);
  game_print(g5_ext);
  assert(game_is_well_paired(g5_ext) == true);

  // The connection is not affected even if an empty shape is added in other
  // places.
  game_set_piece_shape(g5_ext, 2, 2, EMPTY);
  assert(game_is_well_paired(g5_ext) == true);

  /// Replacing one of the cases with a TEE will not work
  game_set_piece_shape(g5_ext, 1, 0, TEE);
  game_set_piece_orientation(g5_ext, 1, 0, EAST);
  assert(game_is_well_paired(g5_ext) == false);

  ////Test if a random shape is added at another location works
  game_set_piece_shape(g5_ext, 1, 0,
                       SEGMENT);  // Modify it so that it connects correctly.
  game_set_piece_orientation(g5_ext, 1, 0, NORTH);
  game_set_piece_shape(g5_ext, 4, 4, ENDPOINT);  // add one for testing
  assert(game_is_well_paired(g5_ext) == false);

  printf("g5_ext test passe\n");
  game_delete(g5_ext);

  // 6. Only one complete connection line
  game g6_ext = game_new_empty_ext(ROWS, COLS, false);
  assert(g6_ext != NULL);
  game_set_piece_shape(g6_ext, 0, 0, ENDPOINT);
  game_set_piece_orientation(g6_ext, 0, 0, EAST);
  game_set_piece_shape(g6_ext, 0, 1, SEGMENT);
  game_set_piece_shape(g6_ext, 0, 2, SEGMENT);
  game_set_piece_shape(g6_ext, 0, 3, SEGMENT);
  game_set_piece_shape(g6_ext, 0, 4, SEGMENT);
  game_set_piece_shape(g6_ext, 0, 5, SEGMENT);
  game_set_piece_shape(g6_ext, 0, 6, SEGMENT);
  game_set_piece_orientation(g6_ext, 0, 1, EAST);
  game_set_piece_orientation(g6_ext, 0, 2, WEST);
  game_set_piece_orientation(g6_ext, 0, 3, WEST);
  game_set_piece_orientation(g6_ext, 0, 4, WEST);
  game_set_piece_orientation(g6_ext, 0, 5, WEST);
  game_set_piece_orientation(g6_ext, 0, 6, WEST);
  game_set_piece_shape(g6_ext, 0, 7, ENDPOINT);
  game_set_piece_orientation(g6_ext, 0, 7, WEST);

  game_print(g6_ext);
  assert(game_is_well_paired(g6_ext) == true);

  /// Replacing one of the cases with a TEE will not work
  game_set_piece_shape(g6_ext, 0, 1, TEE);
  game_set_piece_orientation(g6_ext, 0, 1, NORTH);
  assert(game_is_well_paired(g6_ext) == false);

  printf("g6_ext test passe\n");
  game_delete(g6_ext);

  // 7. Separate connected blocks
  game g7_ext = game_new_empty_ext(ROWS, COLS, false);
  /// Test with one connected block first
  game_set_piece_shape(g7_ext, 0, 0, CORNER);
  game_set_piece_shape(g7_ext, 0, 1, SEGMENT);
  game_set_piece_shape(g7_ext, 0, 2, CORNER);
  game_set_piece_shape(g7_ext, 1, 0, SEGMENT);
  game_set_piece_shape(g7_ext, 1, 2, SEGMENT);
  game_set_piece_shape(g7_ext, 2, 0, CORNER);
  game_set_piece_shape(g7_ext, 2, 1, SEGMENT);
  game_set_piece_shape(g7_ext, 2, 2, CORNER);
  game_set_piece_orientation(g7_ext, 0, 0, EAST);
  game_set_piece_orientation(g7_ext, 0, 2, SOUTH);
  game_set_piece_orientation(g7_ext, 2, 0, NORTH);
  game_set_piece_orientation(g7_ext, 2, 2, WEST);

  game_set_piece_orientation(g7_ext, 0, 1, WEST);
  game_set_piece_orientation(g7_ext, 1, 0,
                             NORTH);               // north south have no différence
  game_set_piece_orientation(g7_ext, 2, 1, EAST);  // west east have no différence
  game_set_piece_orientation(g7_ext, 1, 2, SOUTH);
  game_print(g7_ext);
  assert(game_is_well_paired(g7_ext) == true);
  game_set_piece_shape(g7_ext, 1, 1,
                       SEGMENT);  // Bloc complet connecté avec « segment » au
                                  // milieu du bloc non lié
  game_print(g7_ext);
  assert(game_is_well_paired(g7_ext) == false);
  game_set_piece_shape(g7_ext, 1, 1, EMPTY);
  game_set_piece_shape(g7_ext, 3, 3,
                       ENDPOINT);  // Un bloc connecté complet avec un shape
                                   // aléatoire en extérieur de bloc
  assert(game_is_well_paired(g7_ext) == false);
  game_set_piece_shape(g7_ext, 3, 3, EMPTY);

  /// The other connected block
  game_set_piece_shape(g7_ext, 3, 2, CORNER);
  game_set_piece_shape(g7_ext, 3, 3, CORNER);
  game_set_piece_shape(g7_ext, 4, 2, CORNER);
  game_set_piece_shape(g7_ext, 4, 3, CORNER);
  game_set_piece_orientation(g7_ext, 3, 2, EAST);
  game_set_piece_orientation(g7_ext, 3, 3, SOUTH);
  game_set_piece_orientation(g7_ext, 4, 2, NORTH);
  game_set_piece_orientation(g7_ext, 4, 3, WEST);
  game_print(g7_ext);
  assert(game_is_well_paired(g7_ext) == true);

  game_delete(g7_ext);

  // 8.  Closed-loop trajectory detection using TEE shape instead of CORNER
  game g8_ext = game_new_empty_ext(ROWS, COLS, false);
  assert(g8_ext != NULL);

  game_set_piece_shape(g8_ext, 0, 0, TEE);
  game_set_piece_orientation(g8_ext, 0, 0, SOUTH);

  game_set_piece_shape(g8_ext, 1, 0, SEGMENT);
  game_set_piece_orientation(g8_ext, 1, 0, SOUTH);

  game_set_piece_shape(g8_ext, 2, 0, TEE);
  game_set_piece_orientation(g8_ext, 2, 0, EAST);

  game_set_piece_shape(g8_ext, 2, 1, SEGMENT);
  game_set_piece_orientation(g8_ext, 2, 1, EAST);

  game_set_piece_shape(g8_ext, 2, 2, TEE);
  game_set_piece_orientation(g8_ext, 2, 2, NORTH);

  game_set_piece_shape(g8_ext, 1, 2, SEGMENT);
  game_set_piece_orientation(g8_ext, 1, 2, NORTH);

  game_set_piece_shape(g8_ext, 0, 2, TEE);
  game_set_piece_orientation(g8_ext, 0, 2, WEST);

  game_set_piece_shape(g8_ext, 0, 1, SEGMENT);
  game_set_piece_orientation(g8_ext, 0, 1, WEST);

  game_print(g8_ext);
  assert(game_is_well_paired(g8_ext) == false);

  game_delete(g8_ext);

  //  9.  Check the connection of shapes in different directions in 4 corners
  game g9_ext = game_new_empty_ext(ROWS, COLS, false);
  assert(g9_ext != NULL);
  direction d2[] = {NORTH, SOUTH, WEST, EAST};
  shape s2[] = {CORNER, TEE, SEGMENT, ENDPOINT, CROSS};
  for (int i = 0; i < NB_DIRS; i++) {
    for (int j = 0; j < NB_SHAPES - 1; j++) {
      game_set_piece_shape(g9_ext, 0, 0, s2[j]);
      game_set_piece_orientation(g9_ext, 0, 0, d2[i]);
      assert(game_is_well_paired(g9_ext) == false);
      game_set_piece_shape(g9_ext, 0, 0, EMPTY);

      game_set_piece_shape(g9_ext, game_nb_rows(g9_ext) - 1, 0, s2[j]);
      game_set_piece_orientation(g9_ext, game_nb_rows(g9_ext) - 1, 0, d2[i]);
      assert(game_is_well_paired(g9_ext) == false);
      game_set_piece_shape(g9_ext, game_nb_rows(g9_ext) - 1, 0, EMPTY);

      game_set_piece_shape(g9_ext, 0, game_nb_cols(g9_ext) - 1, s2[j]);
      game_set_piece_orientation(g9_ext, 0, game_nb_cols(g9_ext) - 1, d2[i]);
      assert(game_is_well_paired(g9_ext) == false);
      game_set_piece_shape(g9_ext, 0, game_nb_cols(g9_ext) - 1, EMPTY);

      game_set_piece_shape(g9_ext, game_nb_rows(g9_ext) - 1, game_nb_cols(g9_ext) - 1, s2[j]);
      game_set_piece_orientation(g9_ext, game_nb_rows(g9_ext) - 1, game_nb_cols(g9_ext) - 1, d2[i]);
      assert(game_is_well_paired(g9_ext) == false);
      game_set_piece_shape(g9_ext, game_nb_rows(g9_ext) - 1, game_nb_cols(g9_ext) - 1 - 1, EMPTY);
    }
  }

  game_delete(g9_ext);

  // 10. Check for misaligned connections ( >-  empty  -< )
  game g10_ext = game_new_empty_ext(ROWS, COLS, false);
  game_set_piece_shape(g10_ext, 0, 0, ENDPOINT);
  game_set_piece_orientation(g10_ext, 0, 0, EAST);
  game_set_piece_shape(g10_ext, 0, 2, ENDPOINT);
  game_set_piece_orientation(g10_ext, 0, 2, WEST);
  game_print(g10_ext);
  assert(game_is_well_paired(g10_ext) == false);
  game_delete(g10_ext);

  // 11. Check the CORNER's connection to wall
  game g11_ext = game_new_empty_ext(ROWS, COLS, false);
  game_set_piece_shape(g11_ext, 0, 0, CORNER);
  game_set_piece_orientation(g11_ext, 0, 0, SOUTH);
  game_set_piece_shape(g11_ext, 1, 0, CORNER);
  game_set_piece_orientation(g11_ext, 1, 0, WEST);
  game_set_piece_shape(g11_ext, 0, game_nb_rows(g11_ext) - 1, CORNER);
  game_set_piece_orientation(g11_ext, 0, game_nb_rows(g11_ext) - 1, EAST);
  game_set_piece_shape(g11_ext, 1, game_nb_rows(g11_ext) - 1, CORNER);
  game_set_piece_orientation(g11_ext, 1, game_nb_rows(g11_ext) - 1, NORTH);
  game_print(g11_ext);
  assert(game_is_well_paired(g11_ext) == false);
  game_delete(g11_ext);

  /// ext wrapping
  // 1.game not paired bc (0,0) have orientation west
  uint r = 4, c = 5;
  shape s[] = {TEE, ENDPOINT, ENDPOINT, CORNER,  SEGMENT,  TEE,    ENDPOINT, TEE,      ENDPOINT, CORNER,
               TEE, TEE,      CORNER,   SEGMENT, ENDPOINT, CORNER, CORNER,   ENDPOINT, SEGMENT,  ENDPOINT};
  direction o[] = {NORTH, WEST, EAST, SOUTH, EAST, WEST, EAST, EAST, SOUTH, SOUTH, SOUTH, NORTH, WEST, EAST, EAST, EAST, WEST, NORTH, WEST, WEST};
  direction o_sol[] = {SOUTH, WEST,  SOUTH, NORTH, EAST,  WEST, EAST,  WEST, SOUTH, EAST,
                       EAST,  SOUTH, WEST,  SOUTH, NORTH, WEST, NORTH, WEST, SOUTH, EAST};
  game g1_ext_w = game_new_ext(r, c, s, o, true);
  assert(g1_ext_w != NULL);
  game_print(g1_ext_w);
  assert(game_is_well_paired(g1_ext_w) == false);
  printf("g1_ext_w test passe\n");
  game_delete(g1_ext_w);

  // 2.right game solution is paired
  game g2_ext_w = game_new_ext(r, c, s, o_sol, true);
  assert(g2_ext_w != NULL);
  game_print(g2_ext_w);
  assert(game_is_well_paired(g2_ext_w) == true);
  printf("g2_ext_w test passe\n");
  game_delete(g2_ext_w);

  // 3.mismatched edge so is not paired
  game g3_ext_w = game_new_empty_ext(r, c, true);
  assert(g3_ext_w != NULL);
  game_set_piece_shape(g3_ext_w, 0, 0, ENDPOINT);
  game_set_piece_shape(g3_ext_w, 0, 1, SEGMENT);
  game_set_piece_orientation(g3_ext_w, 0, 0, EAST);
  game_set_piece_orientation(g3_ext_w, 0, 1, EAST);
  assert(game_is_well_paired(g3_ext_w) == false);
  game_print(g3_ext_w);
  printf("g3_ext_w test passe\n");
  game_delete(g3_ext_w);

  // 4.empty game treated as paired
  game g4_ext_w = game_new_empty_ext(r, c, true);
  game_print(g4_ext_w);
  assert(g4_ext_w != NULL);
  assert(game_is_well_paired(g4_ext_w) == true);
  printf("g4_ext_w test passe\n");
  game_delete(g4_ext_w);

  // 5.Only one whole column of connected elements / The effect of the “EMPTY”
  // cell expected : true
  game g5_ext_w = game_new_empty_ext(r, c, true);
  assert(g5_ext_w != NULL);
  game_set_piece_shape(g5_ext_w, 0, 0, ENDPOINT);
  game_set_piece_orientation(g5_ext_w, 0, 0, SOUTH);
  game_set_piece_shape(g5_ext_w, 1, 0, SEGMENT);
  game_set_piece_shape(g5_ext_w, 2, 0, SEGMENT);
  game_set_piece_shape(g5_ext_w, 3, 0, ENDPOINT);
  game_set_piece_orientation(g5_ext_w, 3, 0, NORTH);
  game_print(g5_ext_w);
  assert(game_is_well_paired(g5_ext_w) == true);

  printf("g5_ext_w test passe\n");
  game_delete(g5_ext_w);

  // 6. Only one line all segment
  game g6_ext_w = game_new_empty_ext(r, c, true);
  assert(g6_ext_w != NULL);
  game_set_piece_shape(g6_ext_w, 0, 0, SEGMENT);
  game_set_piece_orientation(g6_ext_w, 0, 0, EAST);
  game_set_piece_shape(g6_ext_w, 0, 1, SEGMENT);
  game_set_piece_shape(g6_ext_w, 0, 2, SEGMENT);
  game_set_piece_shape(g6_ext_w, 0, 3, SEGMENT);
  game_set_piece_shape(g6_ext_w, 0, 4, SEGMENT);
  game_set_piece_orientation(g6_ext_w, 0, 1, EAST);
  game_set_piece_orientation(g6_ext_w, 0, 2, WEST);
  game_set_piece_orientation(g6_ext_w, 0, 3, WEST);
  game_set_piece_orientation(g6_ext_w, 0, 4, WEST);

  game_print(g6_ext_w);
  assert(game_is_well_paired(g6_ext_w) == true);

  printf("g6_ext_w test passe\n");
  game_delete(g6_ext_w);

  // 7. Separate connected blocks
  game g7_ext_w = game_new_empty_ext(r, c, true);
  /// Test with one connected block first
  game_set_piece_shape(g7_ext_w, 0, 0, CORNER);
  game_set_piece_shape(g7_ext_w, 1, 0, CORNER);
  game_set_piece_shape(g7_ext_w, 0, 3, SEGMENT);
  game_set_piece_shape(g7_ext_w, 1, 3, SEGMENT);
  game_set_piece_shape(g7_ext_w, 0, 4, CORNER);
  game_set_piece_shape(g7_ext_w, 1, 4, CORNER);
  game_set_piece_orientation(g7_ext_w, 0, 0, SOUTH);
  game_set_piece_orientation(g7_ext_w, 1, 0, WEST);
  game_set_piece_orientation(g7_ext_w, 0, 3, NORTH);
  game_set_piece_orientation(g7_ext_w, 1, 3, NORTH);
  game_set_piece_orientation(g7_ext_w, 0, 4, EAST);
  game_set_piece_orientation(g7_ext_w, 1, 4, NORTH);

  game_print(g7_ext_w);
  assert(game_is_well_paired(g7_ext_w) == false);
  game_set_piece_shape(g7_ext_w, 0, 3, EMPTY);
  game_set_piece_shape(g7_ext_w, 1, 3, EMPTY);
  game_print(g7_ext_w);
  assert(game_is_well_paired(g7_ext_w) == true);

  /// The other connected block
  game_set_piece_shape(g7_ext_w, 0, 1, CORNER);
  game_set_piece_shape(g7_ext_w, 0, 2, CORNER);
  game_set_piece_shape(g7_ext_w, 3, 1, CORNER);
  game_set_piece_shape(g7_ext_w, 3, 2, CORNER);
  game_set_piece_orientation(g7_ext_w, 0, 1, NORTH);
  game_set_piece_orientation(g7_ext_w, 0, 2, WEST);
  game_set_piece_orientation(g7_ext_w, 3, 1, EAST);
  game_set_piece_orientation(g7_ext_w, 3, 2, SOUTH);

  game_print(g7_ext_w);
  assert(game_is_well_paired(g7_ext_w) == true);
  printf("g7_ext_w test passe\n");
  game_delete(g7_ext_w);

  // 8.  Closed-loop trajectory detection using TEE shape instead of CORNER, expected : false
  game g8_ext_w = game_new_empty_ext(r, c, true);
  assert(g8_ext_w != NULL);

  game_set_piece_shape(g8_ext_w, 0, 0, TEE);
  game_set_piece_orientation(g8_ext_w, 0, 0, WEST);

  game_set_piece_shape(g8_ext_w, 1, 0, SEGMENT);
  game_set_piece_orientation(g8_ext_w, 1, 0, SOUTH);

  game_set_piece_shape(g8_ext_w, 2, 0, TEE);
  game_set_piece_orientation(g8_ext_w, 2, 0, NORTH);

  game_set_piece_shape(g8_ext_w, 0, 4, SEGMENT);
  game_set_piece_orientation(g8_ext_w, 0, 4, EAST);

  game_set_piece_shape(g8_ext_w, 2, 4, SEGMENT);
  game_set_piece_orientation(g8_ext_w, 2, 4, EAST);

  game_set_piece_shape(g8_ext_w, 0, 3, TEE);
  game_set_piece_orientation(g8_ext_w, 0, 3, SOUTH);

  game_set_piece_shape(g8_ext_w, 1, 3, SEGMENT);
  game_set_piece_orientation(g8_ext_w, 1, 3, NORTH);

  game_set_piece_shape(g8_ext_w, 2, 3, TEE);
  game_set_piece_orientation(g8_ext_w, 2, 3, EAST);

  game_print(g8_ext_w);
  assert(game_is_well_paired(g8_ext_w) == false);

  printf("g8_ext_w test passe\n");
  game_delete(g8_ext_w);

  // 9. Check for misaligned connections horizontal ( >-  empty  -< )
  game g9_ext_w = game_new_empty_ext(r, c, true);
  game_set_piece_shape(g9_ext_w, 0, 0, ENDPOINT);
  game_set_piece_orientation(g9_ext_w, 0, 0, EAST);
  game_set_piece_shape(g9_ext_w, 0, 2, ENDPOINT);
  game_set_piece_orientation(g9_ext_w, 0, 2, WEST);
  game_print(g9_ext_w);
  assert(game_is_well_paired(g9_ext_w) == false);
  game_set_piece_shape(g9_ext_w, 0, 2, EMPTY);
  game_set_piece_orientation(g9_ext_w, 0, 0, WEST);
  game_set_piece_shape(g9_ext_w, 0, 3, ENDPOINT);
  game_set_piece_orientation(g9_ext_w, 0, 3, EAST);
  game_print(g9_ext_w);
  assert(game_is_well_paired(g9_ext_w) == false);
  game_set_piece_shape(g9_ext_w, 0, 3, EMPTY);
  game_set_piece_shape(g9_ext_w, 0, 4, ENDPOINT);
  game_set_piece_orientation(g9_ext_w, 0, 4, EAST);
  game_print(g9_ext_w);
  assert(game_is_well_paired(g9_ext_w) == true);
  printf("g9_ext_w test passe\n");
  game_delete(g9_ext_w);

  // 10. Check for misaligned connections vertical ( >-  empty  -< )
  game g10_ext_w = game_new_empty_ext(r, c, true);
  game_set_piece_shape(g10_ext_w, 0, 1, ENDPOINT);
  game_set_piece_orientation(g10_ext_w, 0, 1, NORTH);
  game_set_piece_shape(g10_ext_w, 3, 1, ENDPOINT);
  game_set_piece_orientation(g10_ext_w, 3, 1, SOUTH);
  game_print(g10_ext_w);
  assert(game_is_well_paired(g10_ext_w) == true);
  game_set_piece_shape(g10_ext_w, 0, 1, EMPTY);
  game_set_piece_shape(g10_ext_w, 1, 1, ENDPOINT);
  game_set_piece_orientation(g10_ext_w, 1, 1, NORTH);
  game_print(g10_ext_w);
  assert(game_is_well_paired(g10_ext_w) == false);

  printf("g10_ext_w test passe\n");
  game_delete(g10_ext_w);

  printf("test_game_is_well_paired_ext passed!\n");
  return true;
}

bool test_game_undo() {
  game g = game_default();
  assert(g != NULL);

  direction before_orien = game_get_piece_orientation(g, 1, 0);
  game_play_move(g, 1, 0, 2);

  game_undo(g);
  direction after_undo_orien = game_get_piece_orientation(g, 1, 0);
  printf("before_orien:%d, after_undo_orien:%d, should be equal\n", before_orien, after_undo_orien);
  assert(before_orien == after_undo_orien);

  game_undo(g);

  // test ext wrapping
  game g_ext = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, true);
  before_orien = game_get_piece_orientation(g_ext, 1, 0);

  game_play_move(g_ext, 1, 0, 2);

  game_undo(g_ext);  // undo
  after_undo_orien = game_get_piece_orientation(g_ext, 1, 0);
  assert(before_orien == after_undo_orien);  // check before undo and after undo equal

  game_undo(g_ext);

  game_delete(g);
  game_delete(g_ext);
  printf("test game_undo passed\n");
  return true;
}

bool test_game_redo() {
  game g = game_default();
  assert(g != NULL);
  game_play_move(g, 1, 0, 2);
  direction before_undo_orien = game_get_piece_orientation(g, 1, 0);

  game_undo(g);  // need at least have one element undo to do the redo operation

  game_redo(g);
  direction after_redo_orien = game_get_piece_orientation(g, 1, 0);
  printf("before_undo_orien:%d, after_redo_orien:%d, should be equal\n", before_undo_orien, after_redo_orien);
  assert(before_undo_orien == after_redo_orien);

  game_redo(g);

  // test ext wrapping
  game g_ext = game_new_ext(ROWS, COLS, SHAPES, ORIENTATIONS, true);
  game_play_move(g_ext, 1, 0, 2);
  before_undo_orien = game_get_piece_orientation(g_ext, 1, 0);

  game_undo(g_ext);

  game_redo(g_ext);
  after_redo_orien = game_get_piece_orientation(g_ext, 1, 0);
  assert(before_undo_orien == after_redo_orien);

  game_redo(g_ext);

  game_delete(g);
  game_delete(g_ext);
  printf("test game_redo passed\n");
  return true;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "dummy") == 0) {
    test_dummy();
  }
  if (strcmp(argv[1], "game_default") == 0) {
    test_game_default();
  }

  if (strcmp(argv[1], "game_default_solution") == 0) {
    test_game_default_solution();
  }

  if (strcmp(argv[1], "game_get_ajacent_square") == 0) {
    test_game_get_ajacent_square();
  }

  if (strcmp(argv[1], "game_has_half_edge") == 0) {
    test_game_has_half_edge();
  }

  if (strcmp(argv[1], "game_check_edge") == 0) {
    test_game_check_edge();
  }

  if (strcmp(argv[1], "game_is_well_paired") == 0) {
    test_game_is_well_paired();
  }

  if (strcmp(argv[1], "game_print") == 0) {
    test_game_print();
  }
  if (strcmp(argv[1], "game_undo") == 0) {
    test_game_undo();
  }
  if (strcmp(argv[1], "game_redo") == 0) {
    test_game_redo();
  }
  if (strcmp(argv[1], "game_get_ajacent_square_ext") == 0) {
    test_game_get_ajacent_square_ext();
  }
  if (strcmp(argv[1], "game_has_half_edge_ext") == 0) {
    test_game_has_half_edge_ext();
  }
  if (strcmp(argv[1], "game_check_edge_ext") == 0) {
    test_game_check_edge_ext();
  }
  if (strcmp(argv[1], "game_is_well_paired_ext") == 0) {
    test_game_is_well_paired_ext();
  }
  return EXIT_SUCCESS;
}
