#ifndef MAGICS_EVENT_H
#define MAGICS_EVENT_H

#include <stdbool.h>
#include <SDL.h>
#include "object.h"

void update_button_timers(Button* const buttons, Upgrade* const upgrades, GameState* const state);

void update_incantation_timers(Upgrade* const upgrades, Mix_Chunk** const sounds);

void update_meditate_cooldown(GameState* const state, Mix_Chunk** const sounds);

void update_meditate_timer(GameState* const state);

void update_autosave_timer(Config* const config, int* const autosave_timer);

void update_ending_timers(Config* const config);

bool clicked_button(const Button* const bptr, const SDL_Point* const pos);

bool clicked_upgrade(const Upgrade* const upgrade, const SDL_Point* const pos);

void update_mute_button(Config* const config);

void event_update_mute(Config* const config);

void check_done_buttons(Config* const config, unsigned int* const upgrade_page, unsigned int* const princess_page, bool* const running, unsigned int* save_slot);

bool check_ending_success(const Config* const config);

void check_unlock_menu_button(const int upgrade_offset, Config* const config);

void handle_click_menu_button(const SDL_Point* const mouse_pos, Config* const config);

void handle_click_menu_meditate(const SDL_Point* const mouse_pos, Config* const config);

void handle_click_menu_upgrades(const SDL_Point* const mouse_pos, Config* const config, unsigned int* const upgrade_page, unsigned int* const princess_page);

void handle_click_menu_face_evil(const SDL_Point* const mouse_pos, Config* const config);

void handle_click_save(const SDL_Point* const mouse_pos, Config* const config);

void handle_click_mute_quit(const SDL_Point* mouse_pos, Config* const config);

void handle_click_options(const SDL_Point* mouse_pos, Config* const config);

void handle_click_game_loop(const SDL_Point* const mouse_pos, Config* const config, unsigned int* const upgrade_page, unsigned int* const princess_page);

void handle_click_title(const SDL_Point* mouse_pos, Config* const config);

void handle_click_ending(const SDL_Point* mouse_pos, Config* const config);

void event_confirm_quit(Config* const config);

void handle_event_sdl(const SDL_Event event, SDL_Point* const mouse_pos, Config* const config, 
						bool* const running, unsigned int* const upgrade_page, unsigned int* const princess_page);

#endif