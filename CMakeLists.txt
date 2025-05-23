cmake_minimum_required(VERSION 3.3)
project(game_text C)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -Wall")

## find SDL2
include(sdl2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")
include_directories(${SDL2_ALL_INC})


add_library(game STATIC game.c game_aux.c game_ext.c queue.c game_tools.c game_private.c)

add_executable(game_text game_text.c)
target_link_libraries(game_text game)

add_executable(game_test_famseye game_test_famseye.c)
add_executable(game_test_qxie game_test_qxie.c)
add_executable(game_test_awniang game_test_awniang.c)
add_executable(game_random game_random.c)
add_executable(game_solve game_solve.c)
add_executable(game_sdl main.c game_sdl.c)

target_link_libraries(game_test_famseye game)
target_link_libraries(game_test_qxie game)
target_link_libraries(game_test_awniang game)
target_link_libraries(game_random game)
target_link_libraries(game_solve game)
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} m game)

add_test(test_qxie_dummy ./game_test_qxie dummy)
add_test(test_awniang_dummy ./game_test_awniang dummy)
add_test(test_famseye_dummy ./game_test_famseye dummy)

add_test(test_famseye_play_move ./game_test_famseye game_play_move)
add_test(test_famseye_get_piece_orientation ./game_test_famseye get_piece_orientation)
add_test(test_famseye_get_piece_shape ./game_test_famseye get_piece_shape)
add_test(test_famseye_game_reset_orientation ./game_test_famseye reset_orientation)
add_test(test_famseye_game_shuffle_orientation ./game_test_famseye shuffle_orientation)
add_test(test_famseye_game_won ./game_test_famseye game_won)
add_test(test_famseye_game_is_connected ./game_test_famseye game_is_connected)
add_test(test_famseye_game_is_well_paired ./game_test_famseye game_is_well_paired)
add_test(test_game_nb_rows ./game_test_famseye game_nb_rows)
add_test(test_game_new_empty_ext ./game_test_famseye game_new_empty_ext)
add_test(test_game_new_ext ./game_test_famseye game_new_ext)


add_test(test_awniang_game_new_empty ./game_test_awniang game_new_empty)
add_test(test_awniang_game_new ./game_test_awniang game_new)
add_test(test_awniang_game_equal ./game_test_awniang game_equal)
add_test(test_awniang_game_delete ./game_test_awniang game_delete)
add_test(test_awniang_game_set_piece_shape ./game_test_awniang game_set_piece_shape)
add_test(test_awniang_game_set_piece_orientation ./game_test_awniang game_set_piece_orientation)
add_test(test_awniang_game_copy ./game_test_awniang game_copy)
add_test(test_awniang_game_nb_cols  ./game_test_awniang game_nb_cols)
add_test(test_awniang_game_is_wrapping ./game_test_awniang game_is_wrapping)
add_test(test_awniang_game_load ./game_test_awniang game_load)
add_test(test_awniang_game_save ./game_test_awniang game_save)


add_test(test_qxie_game_default ./game_test_qxie game_default)
add_test(test_qxie_game_default_solution ./game_test_qxie game_default_solution)
add_test(test_qxie_game_get_ajacent_square ./game_test_qxie game_get_ajacent_square)
add_test(test_qxie_game_get_ajacent_square_ext ./game_test_qxie game_get_ajacent_square_ext)
add_test(test_qxie_game_has_half_edge ./game_test_qxie game_has_half_edge)
add_test(test_qxie_game_has_half_edge_ext ./game_test_qxie game_has_half_edge_ext)
add_test(test_qxie_game_check_edge ./game_test_qxie game_check_edge)
add_test(test_qxie_game_check_edge_ext ./game_test_qxie game_check_edge_ext)
add_test(test_qxie_game_is_well_paired ./game_test_qxie game_is_well_paired)
add_test(test_qxie_game_is_well_paired_ext ./game_test_qxie game_is_well_paired_ext)
add_test(test_qxie_game_print ./game_test_qxie game_print)
add_test(test_qxie_game_undo ./game_test_qxie game_undo)
add_test(test_qxie_game_redo ./game_test_qxie game_redo)
#############################################

file(COPY images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
