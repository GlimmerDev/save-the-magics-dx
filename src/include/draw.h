#ifndef MAGICS_DRAW_H
#define MAGICS_DRAW_H

#include <SDL.h>
#include <SDL_FontCache.h>
#include "enum.h"
#include "object.h"

int init_font_paths();
int init_fonts(SDL_Renderer* renderer);

void magics_font_cleanup();

void bg_vertex_cleanup();

int init_tree_shapes();

int init_hill_shapes();

void init_starfield();

void init_bg_shapes();

void init_end_ship_shapes();

void init_shapes(Config* const config);

void draw_starfield(SDL_Renderer* renderer);

void update_starfield(GameState* state);

int clear_screen(SDL_Renderer* renderer, const E_ColorIndex color);

void draw_background_planet(SDL_Renderer* renderer);

void draw_background_meditate(SDL_Renderer* renderer);

int SDL_RenderDrawCircle(SDL_Renderer * renderer, const int x, const int y, const int radius);

void SDL_RenderFillRing(SDL_Renderer *renderer, int cx, int cy, int outerRadius, int innerRadius);

int SDL_RenderFillCircle(SDL_Renderer * renderer, const int x, const int y, const int radius);

void SDL_RenderFillTriangle(SDL_Renderer* renderer, const E_ColorIndex color, int x1, int y1, int x2, int y2, int x3, int y3);

int draw_rect(const SDL_Rect* rect, const E_ColorIndex color, SDL_Renderer* renderer);

int _draw_button(Config* config, bool set_ptrs, bool by_ref, const E_ButtonIndex b_index, Button* b_ref);
int draw_button(const E_ButtonIndex b);
int draw_button_by_ref(Button* bptr);
int draw_button_set_ptrs(Config* cptr);

void draw_upgrade_text(const Upgrade* upgrade, const GameState* state, SDL_Renderer* renderer);

int draw_upgrade(const Upgrade* upgrade, const int slot, SDL_Renderer* renderer, const GameState* state);

int set_draw_color(SDL_Renderer* renderer, const E_ColorIndex color);

void draw_text(const char* text, const int x, const int y, const int font, 
				const int size, const E_ColorIndex color, SDL_Renderer* renderer);

void draw_magics_info(SDL_Renderer* renderer, Button* buttons, GameState* state);

void draw_menu_buttons(SDL_Renderer* renderer, Button* buttons);

void draw_menu_upgrades(SDL_Renderer* renderer, Config* config, unsigned int* upgrade_page, unsigned int* princess_page);

void draw_menu_meditate(Config* config);

void draw_menu_face_evil(SDL_Renderer* renderer, Config* config);

void draw_screen_ending(SDL_Renderer* renderer, Config* config);

void draw_screen_title(Config* config);

void draw_option_aspect(const Config* const config);
void draw_option_fps(const Config* const config);
void draw_option_autosave(const Config* const config);
void draw_option_import(const Config* const config);

void draw_screen_options(Config* config);

void draw_screen_save(Config* config);

void draw_screen_game_loop(Config* config, unsigned int* upgrade_page, unsigned int* princess_page);

void draw_background(Config* config);

#endif