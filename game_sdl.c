#include "game_sdl.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"
#include "game_tools.h"

#define FONT "images/arial.ttf"
#define FONTSIZE 24
#define BACKGROUND "images/background.jpg"
#define IMG_CORNER "images/corner.png"
#define IMG_TEE "images/tee.jpg"
#define IMG_SEGMENT "images/segment.jpg"
#define IMG_CROSS "images/cross.jpg"
#define IMG_EMPTY "images/empty.jpg"
#define IMG_ENDPOINT "images/endpoint.png"

/* **************************************************************** */
typedef struct {
  SDL_Texture *texture;
  SDL_Rect rect;
} LTexture;

enum UIType { UI_NULL, UI_BUTTON };

enum UIStatus {
  UI_CLICKED,
  UI_RELEASED,
};

typedef enum { BTN_GAME_TYPE, BTN_UNDO, BTN_REDO, BTN_GAME_SOLVE, BTN_GAME_SAVE } ButtonName;

typedef struct {
  int type;
  int status;
  SDL_Rect rect;
  LTexture ltexture;
  char *text;
  ButtonName btn_name;
} UIButton;

typedef struct {
  int x, y;
} coords;

struct Env_t {
  game game;
  SDL_Texture *background;
  SDL_Texture *corner;
  SDL_Texture *tee;
  SDL_Texture *segment;
  SDL_Texture *cross;
  SDL_Texture *endpoint;
  SDL_Texture *empty;
  coords **coord;
  int cols, rows;
  // for btns
  UIButton **btns;
  int nb_btn;
  // for input
  bool get_user_input;
  char *intext;
  // message help
  char *help;
  SDL_Texture *win_msg;
  SDL_Texture *down_msg;
};

/* **************************************************************** */

// draw buttons border
void draw_back_btn(SDL_Renderer *ren, int x1, int y1, int x2, int y2, int thick, int dark, int bright) {
  int k;
  for (k = 0; k < thick; k++) {
    SDL_SetRenderDrawColor(ren, dark, dark, dark, 255);
    SDL_RenderDrawLine(ren, x1 + k, y1 + k, x2 - k, y1 + k);
    SDL_RenderDrawLine(ren, x1 + k, y1 + k, x1 + k, y2 - k);
    SDL_SetRenderDrawColor(ren, bright, bright, bright, 255);
    SDL_RenderDrawLine(ren, x1 + k, y2 - k, x2 - k, y2 - k);
    SDL_RenderDrawLine(ren, x2 - k, y1 + k, x2 - k, y2 - k);
  }
}

// draw button
void draw_button(UIButton *b, SDL_Renderer *ren) {
  int bright = 200, dark = 100, grey = (bright + dark) / 2;
  int thick = b->rect.h / 10;

  // put button in grey
  SDL_SetRenderDrawColor(ren, grey, grey, grey, 255);
  SDL_RenderFillRect(ren, &b->rect);

  // changing appearence when clicked or not
  if (b->status == UI_CLICKED) {
    draw_back_btn(ren, b->rect.x, b->rect.y, b->rect.x + b->rect.w, b->rect.y + b->rect.h, thick, dark, bright);
  }
  if (b->status == UI_RELEASED) {
    draw_back_btn(ren, b->rect.x, b->rect.y, b->rect.x + b->rect.w, b->rect.y + b->rect.h, thick, bright, dark);
  }
}

// draw inputbox
void draw_input_box(SDL_Renderer *ren, Env *env, int w, int h) {
  // render style setting (position ,size, color)
  SDL_Rect inputbox = {w / 4, h / 4, 350, 50};
  SDL_SetTextInputRect(&inputbox);
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, &inputbox);
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderDrawRect(ren, &inputbox);

  // render text
  SDL_Color color = {0, 0, 0, 255};
  TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
  if (strlen(env->intext) > 0) {
    SDL_Surface *surf = TTF_RenderText_Blended(font, env->intext, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surf);

    // putting text inside the input box
    SDL_Rect text_rect = {inputbox.x + 5, inputbox.y + 5, surf->w, surf->h};
    SDL_RenderCopy(ren, texture, NULL, &text_rect);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(texture);
  }
  TTF_CloseFont(font);
}

UIButton *init_btn(UIButton *b, SDL_Renderer *ren, TTF_Font *font, char *text, SDL_Color color, SDL_Rect rect, ButtonName name) {
  // button specifications (not clicked by default, position, name)
  b->status = UI_RELEASED;
  b->type = UI_BUTTON;
  b->rect.x = rect.x;
  b->rect.y = rect.y;
  b->rect.w = rect.w;
  b->rect.h = rect.h;
  SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
  b->ltexture.texture = SDL_CreateTextureFromSurface(ren, surf);
  b->ltexture.rect.h = surf->h;
  b->ltexture.rect.w = surf->w;
  b->btn_name = name;
  SDL_FreeSurface(surf);
  return b;
}

// When widows resized buttuns stay centerd
void update_button_positions(Env *env, int w) {
  int btn_width = 150, spacing = 20;
  int total_width = env->nb_btn * btn_width + (env->nb_btn - 1) * spacing;
  int start_x = (w - total_width) / 2, start_y = 20;

  for (int i = 0; i < env->nb_btn; i++) {
    env->btns[i]->rect.x = start_x + i * (btn_width + spacing);
    env->btns[i]->rect.y = start_y;
  }
}

/*free coords*/
void free_coord(Env *env) {
  if (env->coord) {
    for (int i = 0; i < env->rows; i++) {
      free(env->coord[i]);
    }
    free(env->coord);
  }
}

/*set coords*/
void set_coord(Env *env, SDL_Window *win, int rows, int cols) {
  env->rows = rows;
  env->cols = cols;
  // redefine coord
  env->coord = malloc(env->rows * sizeof(coords *));
  for (int i = 0; i < env->rows; i++) {
    env->coord[i] = malloc(env->cols * sizeof(coords));
  }
  // update coord
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  for (int i = 0; i < env->rows; i++) {
    for (int j = 0; j < env->cols; j++) {
      env->coord[i][j].x = (w / env->cols) * j;
      env->coord[i][j].y = (h / env->rows) * i;
    }
  }
}

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  game g = NULL;

  // load game_default or chosen game
  if (argc == 1)
    g = game_default();
  else if (argc == 2)
    g = game_load(argv[1]);
  env->game = g;

  /*set font*/
  SDL_Color color = {0, 0, 255, 255}; /* blue color in RGBA */
  TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);  // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL

  // number of textures
  int size = 7;

  // All the textures in the structure env
  const char *texture_paths[] = {BACKGROUND, IMG_CORNER, IMG_TEE, IMG_SEGMENT, IMG_CROSS, IMG_ENDPOINT, IMG_EMPTY};

  SDL_Texture **textures[] = {&env->background, &env->corner, &env->tee, &env->segment, &env->cross, &env->endpoint, &env->empty};

  // Loading all the textures
  for (int i = 0; i < size; i++) {
    *textures[i] = IMG_LoadTexture(ren, texture_paths[i]);
    if (!*textures[i]) {
      ERROR("IMG_LoadTexture: %s\n", texture_paths[i]);
    }
  }

  int cols = game_nb_cols(g);
  int rows = game_nb_rows(g);

  set_coord(env, win, rows, cols);

  /*init btns & intext &  help message*/
  env->help =
      " Click left or right buttons to play move.\n"
      "Press key:\n"
      "- [n] new random game\n"
      "- [r] restart (shuffle)\n"
      "- [s] solve\n"
      "- [z] undo\n"
      "- [y] redo\n"
      "- [p] print\n"
      "- [e] toggle errors\n"
      "- [h] show help\n"
      "- [q] quit\n"
      "Save mode:\n"
      "- [esc] close save mode\n"
      "- [enter] save game in file\n";

  SDL_Surface *surf = TTF_RenderText_Blended_Wrapped(font, env->help, color, 600);  // 600 = largeur max pour le wrapping
  if (!surf) ERROR("TTF_RenderText_Blended_Wrapped: %s\n", TTF_GetError());

  // no input by default
  env->get_user_input = false;

  // intext field
  env->intext = malloc(sizeof(char) * 100);
  if (!env->intext) ERROR("err malloc title\n");
  env->intext[0] = '\0';

  // number of buttons
  env->nb_btn = 5;

  // Allocating storage for all buttons
  env->btns = malloc(sizeof(UIButton *) * env->nb_btn);
  if (!env->btns) ERROR("malloc env->btns\n");

  // buttons specification
  char *btntexts[] = {"New random", "Undo move", "Redo move", "Solve game", "Save game"};
  int btn_width = 150, btn_height = 50, spacing = 20;
  int total_width = env->nb_btn * btn_width + (env->nb_btn - 1) * spacing;
  int start_x = (w - total_width) / 2, start_y = 20;

  // buttons creation
  for (int i = 0; i < env->nb_btn; i++) {
    SDL_Rect rect = {start_x + i * (btn_width + spacing), start_y, btn_width, btn_height};
    env->btns[i] = malloc(sizeof(UIButton));
    if (!env->btns[i]) ERROR("malloc env->btns[i], %s\n", btntexts[i]);
    env->btns[i] = init_btn(env->btns[i], ren, font, btntexts[i], color, rect, i);
  }

  // down screen msg
  surf = TTF_RenderText_Blended(font, "Press [h] for help !", color);
  env->down_msg = SDL_CreateTextureFromSurface(ren, surf);

  // win msg
  font = TTF_OpenFont(FONT, FONTSIZE * 5);
  color = (SDL_Color){255, 0, 0, 255};
  surf = TTF_RenderText_Blended(font, "You win !", color);
  env->win_msg = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);
  return env;
}

/* **************************************************************** */
// render btn
void render_btn(UIButton *b, SDL_Renderer *ren, SDL_Rect *rect) {
  SDL_QueryTexture(b->ltexture.texture, NULL, NULL, &rect->w, &rect->h);
  draw_button(b, ren);
  rect->x = b->rect.x + (b->rect.w - b->ltexture.rect.w) / 2;
  rect->y = b->rect.y + (b->rect.h - b->ltexture.rect.h) / 2;
  SDL_RenderCopy(ren, b->ltexture.texture, NULL, rect);
}

// calculate grid value
void calc_grid(SDL_Renderer *ren, int cols, int rows, int *cell_w, int *cell_h, int *need_w, int *need_h, int *start_x, int *start_y,
               int *cell_size) {
  int w, h;
  SDL_GetRendererOutputSize(ren, &w, &h);
  // Définir la zone de la grille (ex : 80% de la fenêtre)
  float scale = 0.8;
  int grid_w = w * scale;
  int grid_h = h * scale;

  // size and be a square
  *cell_w = grid_w / cols;
  *cell_h = grid_h / rows;
  *cell_size = (*cell_w < *cell_h) ? *cell_w : *cell_h;

  // needed size
  *need_w = (*cell_size) * cols;
  *need_h = (*cell_size) * rows;

  // begin (x,y)
  *start_x = (w - *need_w) / 2;
  *start_y = (h - *need_h) / 2;
}

void create_grid(Env *env, SDL_Renderer *ren, int cols, int rows, int start_x, int start_y, int need_h, int need_w, int cell_size) {
  // Dessiner les lignes verticales
  int y1 = start_y, y2 = y1 + need_h;
  for (int i = 0; i <= cols; i++) {
    int x1 = start_x + i * cell_size, x2 = x1;
    SDL_RenderDrawLine(ren, x1, y1, x2, y2);
  }

  // Dessiner les lignes horizontales
  int x1 = start_x, x2 = x1 + need_w;
  for (int i = 0; i <= rows; i++) {
    int y1 = start_y + i * cell_size, y2 = y1;
    SDL_RenderDrawLine(ren, x1, y1, x2, y2);
  }
}

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_Rect rect;
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_RenderCopy(ren, env->background, NULL, NULL); /* stretch it */
  // get grid value & create grid
  int cell_w, cell_h, need_w, need_h, start_x, start_y, cell_size;
  calc_grid(ren, env->cols, env->rows, &cell_w, &cell_h, &need_w, &need_h, &start_x, &start_y, &cell_size);
  create_grid(env, ren, env->cols, env->rows, start_x, start_y, need_h, need_w, cell_size);
  for (int i = 0; i < env->rows; i++) {
    for (int j = 0; j < env->cols; j++) {
      shape p = game_get_piece_shape(env->game, i, j);
      SDL_Texture *texture = NULL;
      // Déterminer le type de la pièce
      switch (p) {
        case CORNER:
          texture = env->corner;
          break;
        case TEE:
          texture = env->tee;
          break;
        case ENDPOINT:
          texture = env->endpoint;
          break;
        case CROSS:
          texture = env->cross;
          break;
        case SEGMENT:
          texture = env->segment;
          break;
        default:
          texture = env->empty;
          break;
      }

      // la position de la pièce sur la grille
      SDL_Rect dest = {start_x + j * cell_size, start_y + i * cell_size, cell_size, cell_size};

      // Afficher la pièce
      if (texture) {
        int angle = game_get_piece_orientation(env->game, i, j) * 90;
        SDL_RenderCopyEx(ren, texture, NULL, &dest, angle, NULL, SDL_FLIP_NONE);
      }
    }
  }

  /*render buttons*/
  for (int i = 0; i < env->nb_btn; i++) render_btn(env->btns[i], ren, &rect);

  /*render input box*/
  if (env->get_user_input) {
    draw_input_box(ren, env, w, h);
  }

  /*render down msg*/
  SDL_QueryTexture(env->down_msg, NULL, NULL, &rect.w, &rect.h);
  rect.x = (w - rect.w) / 2;
  rect.y = h - 50;
  SDL_RenderCopy(ren, env->down_msg, NULL, &rect);

  /*render win msg*/
  if (game_won(env->game)) {
    SDL_QueryTexture(env->win_msg, NULL, NULL, &rect.w, &rect.h);
    rect.x = (w - rect.w) / 2;
    rect.y = (h - rect.h) / 2;
    SDL_RenderCopy(ren, env->win_msg, NULL, &rect);
  }
}

/* **************************************************************** */
// handle input event
void handle_input(Env *env, SDL_Event *e) {
  if (e->type == SDL_TEXTINPUT) strncat(env->intext, e->text.text, sizeof(env->intext) - strlen(env->intext) - 1);

  if (e->type == SDL_KEYDOWN) {
    SDL_Keycode k = e->key.keysym.sym;
    // close inputbox
    if (k == SDLK_ESCAPE) {
      SDL_StopTextInput();
      env->get_user_input = false;
    }
    // errase last character
    if (k == SDLK_BACKSPACE) {
      int l = strlen(env->intext);
      if (l > 0) env->intext[l - 1] = '\0';
    }
    // writing done
    if (k == SDLK_RETURN) {
      SDL_StopTextInput();
      env->get_user_input = false;
      if (!env->intext || strlen(env->intext) == 0) {
        // default saving path
        game_save(env->game, "saved_game");
      } else {
        game_save(env->game, env->intext);
        // errase the input after its use
        env->intext[0] = '\0';
      };
    }
  }
}

void handle_random(SDL_Window *win, Env *env) {
  // get new random game
  int new_rows = rand() % 10 + 1;
  int new_cols = rand() % 10 + 1;
  int wrapping = rand() % 2;
  int nb_empty = rand() % 3;
  int nb_extra = rand() % 3;
  game_delete(env->game);
  env->game = game_random(new_rows, new_cols, wrapping, nb_empty, nb_extra);
  if (env->game == NULL) {
    env->game = game_random(new_rows, new_cols, wrapping, 0, 0);
    if (env->game == NULL) {
      env->game = game_default();
    }
  }
  game_shuffle_orientation(env->game);

  // free old coord
  free_coord(env);

  // set new coord
  set_coord(env, win, new_rows, new_cols);
}

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  int x, y;
  SDL_GetMouseState(&x, &y);

  /* quit */
  if (e->type == SDL_QUIT) return true;

  /*resize event*/
  if (e->type == SDL_WINDOWEVENT) {
    if (e->window.event == SDL_WINDOWEVENT_RESIZED) {
      int new_w, new_h;
      SDL_GetWindowSize(win, &new_w, &new_h);
      update_button_positions(env, new_w);
    }
  }

  /*btn events & only check btn when btns area being touched */
  int btn_area = env->btns[0]->rect.y + env->btns[0]->rect.h;

  if (y >= 0 && y <= btn_area) {
    for (int i = 0; i < env->nb_btn; i++) {
      SDL_Rect buttonRect = (SDL_Rect){env->btns[i]->rect.x, env->btns[i]->rect.y, env->btns[i]->rect.w, env->btns[i]->rect.h};
      bool on_topbtn = x >= buttonRect.x && x <= buttonRect.x + buttonRect.w && y <= buttonRect.y + buttonRect.h && y >= buttonRect.y;
      /*btn clicked event*/
      if (e->type == SDL_MOUSEBUTTONDOWN)
        if (on_topbtn) env->btns[i]->status = UI_CLICKED;

      /*btn released event*/
      if (e->type == SDL_MOUSEBUTTONUP) {
        if (on_topbtn) {
          env->btns[i]->status = UI_RELEASED;
          ButtonName n = env->btns[i]->btn_name;

          /////things to be done after pressed btn
          if (n == BTN_UNDO) game_undo(env->game);
          if (n == BTN_REDO) game_redo(env->game);
          if (n == BTN_GAME_SOLVE) {
            game_solve(env->game);
            render(win, ren, env);
            SDL_RenderPresent(ren);
          }
          if (n == BTN_GAME_SAVE) {
            SDL_StartTextInput();
            env->get_user_input = true;
          }
          if (n == BTN_GAME_TYPE) handle_random(win, env);
        }
      }
    }
  }

  /* handle input event*/
  if (env->get_user_input) handle_input(env, e);

  /*handle event clavier*/
  if (e->type == SDL_KEYDOWN && !env->get_user_input) {
    // 9 keys to be detected
    SDL_Keycode k = e->key.keysym.sym;
    /* z -> undo*/
    if (k == SDLK_z) game_undo(env->game);
    /* y -> redo*/
    if (k == SDLK_y) game_redo(env->game);
    /* r -> shuffle*/
    if (k == SDLK_r) game_shuffle_orientation(env->game);
    /* q -> quit*/
    if (k == SDLK_q) return true;
    /* h -> help*/
    if (k == SDLK_h) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Help", env->help, win);
    /* s -> solve*/
    if (k == SDLK_s) {
      game_solve(env->game);
      render(win, ren, env);
      SDL_RenderPresent(ren);
    }
    /* p -> print*/
    if (k == SDLK_p) game_print(env->game);
    /* e -> toggle_errors*/
    if (k == SDLK_e) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "key 'e' is not available\n", win);
    /* n -> new random game*/
    if (k == SDLK_n) handle_random(win, env);
  }

  // handle game operation
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    // get grid info
    int start_x, start_y, cell_size, need_w, need_h, cell_w, cell_h;
    calc_grid(ren, env->cols, env->rows, &cell_w, &cell_h, &need_w, &need_h, &start_x, &start_y, &cell_size);
    int end_x = start_x + need_w, end_y = start_y + need_h;

    // Vérifie que le clic est dans la grille
    if (x >= start_x && x <= end_x && y >= start_y && y <= end_y) {
      int col = (x - start_x) / cell_size, row = (y - start_y) / cell_size;
      if (e->button.button == SDL_BUTTON_LEFT) {
        game_play_move(env->game, row, col, 1);
      } else if (e->button.button == SDL_BUTTON_RIGHT) {
        game_play_move(env->game, row, col, -1);
      }
      render(win, ren, env);
      SDL_RenderPresent(ren);
    }
  }
  return false;
}

/* **************************************************************** */

//
void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  if (!env) return;
  // All the textures in the structure env
  SDL_Texture *textures[] = {env->background, env->corner, env->tee,      env->segment, env->cross,
                             env->endpoint,   env->empty,  env->down_msg, env->win_msg};

  // The number of textures
  int size = 9;

  for (int i = 0; i < size; i++) {
    if (textures[i]) {
      SDL_DestroyTexture(textures[i]);
    }
  }

  free_coord(env);

  // Free all the buttons
  if (env->btns) {
    for (int i = 0; i < env->nb_btn; i++) {
      if (env->btns[i]) {
        if (env->btns[i]->ltexture.texture) {
          SDL_DestroyTexture(env->btns[i]->ltexture.texture);
        }
        free(env->btns[i]);
      }
    }
    free(env->btns);
  }

  if (env->game) {
    game_delete(env->game);
  }

  if (env->intext != NULL) free(env->intext);
  free(env);
}

/* **************************************************************** */
