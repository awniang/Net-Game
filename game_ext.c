/**
 * @file game_ext.c
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/

#include "game_ext.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_private.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                                 GAME EXT                                   */
/* ************************************************************************** */

game game_new_ext(uint nb_rows, uint nb_cols, shape* shapes, direction* directions, bool wrapping) {
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  assert(g);
  shape s = EMPTY;
  direction d = NORTH;

  // set squares
  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++) {
      if (shapes != NULL) s = shapes[i * nb_cols + j];
      if (directions != NULL) d = directions[i * nb_cols + j];
      SHAPE(g, i, j) = s;
      ORIENTATION(g, i, j) = d;
    }

  return g;
}

/* ************************************************************************** */

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping) {
  game g = (game)malloc(sizeof(struct game_s));
  assert(g);
  g->nb_rows = nb_rows;
  g->nb_cols = nb_cols;
  g->wrapping = wrapping;
  g->squares = (square*)calloc(g->nb_rows * g->nb_cols, sizeof(square));
  assert(g->squares);
  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++) {
      SHAPE(g, i, j) = EMPTY;
      ORIENTATION(g, i, j) = NORTH;
    }

  // initialize history
  g->undo_stack = queue_new();
  assert(g->undo_stack);
  g->redo_stack = queue_new();
  assert(g->redo_stack);

  return g;
}

/* ************************************************************************** */

uint game_nb_rows(cgame g) { return g->nb_rows; }

/* ************************************************************************** */

uint game_nb_cols(cgame g) { return g->nb_cols; }

/* ************************************************************************** */

bool game_is_wrapping(cgame g) { return g->wrapping; }

/* ************************************************************************** */

void game_undo(game g) {
  assert(g);
  if (_stack_is_empty(g->undo_stack)) return;
  move m = _stack_pop_move(g->undo_stack);
  game_set_piece_orientation(g, m.i, m.j, m.old);
  _stack_push_move(g->redo_stack, m);
}

/* ************************************************************************** */

void game_redo(game g) {
  assert(g);
  if (_stack_is_empty(g->redo_stack)) return;
  move m = _stack_pop_move(g->redo_stack);
  game_set_piece_orientation(g, m.i, m.j, m.new);
  _stack_push_move(g->undo_stack, m);
}

/* ************************************************************************** */
