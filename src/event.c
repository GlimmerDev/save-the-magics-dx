#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/event.h"
#include "include/object.h"
#include "include/save.h"
#include "include/util.h"
#include <SDL.h>

extern const int AUTOSAVE_INTERVALS[];

void update_ending_timers(Config* const config) {
	EndState* ending = config->ending_state;
	if (config->state->current_screen != SCREEN_ENDING) {
		return;
	}
	if (config->state->current_menu != MENU_END_WIN1) {
		return;
	}

	if (ending->explosion_delay < 1) {
		if (ending->explosion_radius < 8000) {
			ending->explosion_radius += (400.0f/get_fps());
		}
		if (ending->explosion_alpha > 1) {
			ending->explosion_alpha -= (22.5f/get_fps());
		}
		if (ending->explosion_delay < (get_fps() * -15)) {
			ending->explosion_delay = (18600/get_fps());
			ending->explosion_alpha = 255;
			ending->explosion_radius = 2;
			config->state->current_menu = MENU_END_WIN2;
		}
	} else {
		if (ending->explosion_delay == (4*get_fps())-1) {
			Mix_PlayChannel(-1, config->sounds[END_SHOOT_SND], 0);
		} else if (ending->explosion_delay == 2) {
			Mix_PlayChannel(-1, config->sounds[END_EXPLODE_SND], 0);
		}
		if (ending->ship_glow_alpha > -150) {
			ending->ship_glow_alpha -= (60/get_fps());
		} else {
			ending->ship_glow_alpha = 150;
		}
	}
	--(ending->explosion_delay);
}

void update_button_timers(Button* const buttons, Upgrade* const upgrades, GameState* const state) {
	for (int i = 0; i < NUM_MISC_BUTTONS; ++i) {
		if (buttons[i].state == STATE_BTN_IDLE) continue;
		if (buttons[i].click_timer > 0) {
			buttons[i].click_timer -= get_fps();
		} else {
			buttons[i].state = STATE_BTN_IDLE;
		}
	}
	for (int i = 0; i < NUM_UPGRADES+NUM_PRINCESSES+NUM_INCANTATIONS; ++i) {
		if (upgrades[i].button->state == STATE_BTN_IDLE) continue;
		if (upgrades[i].button->click_timer > 0) {
			upgrades[i].button->click_timer -= get_fps();
		} else {
			upgrades[i].button->state = STATE_BTN_IDLE;
		}
	}
}

void update_incantation_timers(Upgrade* const upgrades, Mix_Chunk** const sounds) {
	bool play_sound = false;
	for (int i = 0; i < NUM_INCANTATIONS; ++i) {
		if (upgrades[i+INCANTATION_OFFSET].cooldown > 0) {
			--upgrades[i+INCANTATION_OFFSET].cooldown;
		} else if (upgrades[i+INCANTATION_OFFSET].button->locked) {
			upgrades[i+INCANTATION_OFFSET].button->locked = 0;
			play_sound = true;
		}
	}
	if (!play_sound) {
		return;
	}
	// play once for all incantations, to avoid stacking SFX (like on game load)
	Mix_PlayChannel(-1, sounds[INCANT_READY_SND], 0);
}

void update_meditate_cooldown(GameState* const state, Mix_Chunk** const sounds) {
	if (state->meditate_cooldown < 1) {
		return;
	} else if (state->meditate_cooldown == 1) {
		Mix_PlayChannel(-1, sounds[MEDITATE_SND], 0);
		get_button(START_MEDI_B)->locked = false;
	}
	--(state->meditate_cooldown);
}

void update_meditate_timer(GameState* const state) {
	if (state->meditate_timer < 1) {
		state->is_meditating = false;
		state->meditate_timer = DEFAULT_MEDI_TIMER * get_fps();
		return;
	}
	--(state->meditate_timer);
}

void update_autosave_timer(Config* const config, unsigned int* const autosave_timer) {
	if (config->autosave_interval == 0) {
		return;
	}
	++(*autosave_timer);
	unsigned int interval = AUTOSAVE_INTERVALS[config->autosave_interval]*get_fps();
	if ((*autosave_timer) > (interval*get_fps())) {
		*autosave_timer = 0;
		save_game(config, 0);
	}
}

bool clicked_button(const Button* const bptr, const SDL_Point* const pos) {
	if (bptr->state == STATE_BTN_CLICKED) {
		return false;
	}
	return SDL_PointInRect(pos, &(bptr->rect));
}

bool clicked_upgrade(const Upgrade* const upgrade, const SDL_Point* const pos) {
	return clicked_button(upgrade->button, pos);
}

void update_mute_button(Config* const config) {
	Button* bptr = get_button(MUTE_B);
	if (config->state->is_muted) {
		bptr->color = B_RED;
		bptr->name = "Unmute";
	} else {
		bptr->color = B_GREEN;
		bptr->name = "Mute";
	}
}

void event_update_mute(Config* const config) {
	Mix_Volume(-1, MIX_MAX_VOLUME * (!config->state->is_muted));
	update_mute_button(config);
}

void check_done_buttons(Config* const config, unsigned int* const upgrade_page, unsigned int* const princess_page, bool* const running, unsigned int* save_slot) {
	Button* buttons = config->buttons;
	GameState* state = config->state;

	Button* bptr;

	switch (state->current_screen) { 
		case SCREEN_TITLE:
			bptr = get_button(NEW_GAME_B);
			if  (done_button(bptr)) {
				state->current_screen = SCREEN_GAME_LOOP;
			} 
			bptr = get_button(LOAD_GAME_B);
			if (done_button(bptr)) {
				state->current_screen = SCREEN_SAVE;
				state->current_menu = MENU_LD_SLOT;
				check_for_saves(config);
			} 
			bptr = get_button(OPTIONS_B);
			if (done_button(bptr)) {
				state->current_screen = SCREEN_OPTIONS;
			}
			bptr = get_button(COMPENDIUM_B);
			if (done_button(bptr)) {
				*upgrade_page = 0;
				state->current_screen = SCREEN_COMPENDIUM;
			}
			break;
		case SCREEN_OPTIONS:
			bptr = get_button(OPT_CONFIRM_B);
			if (done_button(bptr)) {
				if (config->reload_state > RELOAD_STATE_NONE) {
					create_config_file(config->autosave_interval, config->FPS, 
										config->quitonsave, config->aspect);
					if (config->reload_state == RELOAD_STATE_REQUESTED) {
						++(config->reload_state);
					} else {
						config->reload_state = RELOAD_STATE_NONE;
					}
				}
				config->state->current_screen = SCREEN_TITLE;
			}
			// Autosave can be changed without reload
			bptr = get_button(OPT_AUTOSAVE_B);
			if (done_button(bptr)) {
				config->reload_state = RELOAD_STATE_WRITECFG;
				++config->autosave_interval;
				if (config->autosave_interval > 5) {
					config->autosave_interval = 0;
				}
			}
		#ifdef __MAGICSMOBILE__
			break;
		#else
			bptr = get_button(OPT_ASPECT_B);
			if (done_button(bptr)) {
				config->aspect = !config->aspect;
				config->reload_state = RELOAD_STATE_REQUESTED;
			}
			bptr = get_button(OPT_FPS_B);
			if (done_button(bptr)) {
				config->FPS *= 2;
				if (config->FPS > 120.0) {
					config->FPS = 30.0;
				}
				config->reload_state = RELOAD_STATE_REQUESTED;
			}
			bptr = get_button(OPT_POSTSAVE_B);
			if (done_button(bptr)) {
				config->quitonsave = !(config->quitonsave);
			}
			break;
		#endif
		case SCREEN_SAVE:
			if (state->current_menu == MENU_SV_CNF_OVERWR) {
				bptr = get_button(SAVE_YES_B);
				if (done_button(bptr)) {
					save_game(config, *save_slot);
					if (config->quitonsave) {
						*running = false;
					} else {
						config->reload_state = RELOAD_STATE_EXECUTE_SOFT;
					}	
					return;
				}
				bptr = get_button(SAVE_NO_B);
				if (done_button(bptr)) {
					check_for_saves(config);
					state->current_menu = MENU_SV_SLOT;
				}
				break;
			}
			else if (state->current_menu >= MENU_SV_CNF_QUIT) {
				bptr = get_button(SAVE_YES_B);
				if (done_button(bptr)) {
					check_for_saves(config);
					state->current_menu = MENU_SV_SLOT;
				}
				bptr = get_button(SAVE_NO_B);
				if (done_button(bptr)) {
					if (config->quitonsave) {
						*running = false;
					} else {
						config->reload_state = RELOAD_STATE_EXECUTE_SOFT;
					}
					return;
				}
				bptr = get_button(SAVE_CANCEL_B);
				if (done_button(bptr)) {
					state->current_screen = SCREEN_GAME_LOOP;
					if (config->state->is_meditating) {
						state->current_menu = MENU_MEDITATE;
					} else {
						state->current_menu = MENU_UPGRADES;
					}
				}
				break;
			}
			for (int i = 0; i < 4; ++i) {
				bptr = get_button(SAVE_0_B+i);
				if (done_button(bptr)) {
					// loading
					if (state->current_menu == MENU_LD_SLOT) {
						load_save(config, i);
						state->current_screen = SCREEN_GAME_LOOP;
						state->current_menu = MENU_UPGRADES;
					} 
					// saving
					else if (config->saves[i].exists) {
						*save_slot = i;
						state->current_menu = MENU_SV_CNF_OVERWR;
					} else {
						save_game(config, i);
						*running = false;
						return;
					} 
				}
			}
			break;
		case SCREEN_GAME_LOOP:
			bptr = get_button(MUTE_B);
			if (done_button(bptr)) {
				state->is_muted = !(state->is_muted);
				event_update_mute(config);
			}
			bptr = &(buttons[NEXT_B]);
			if (done_button(bptr)) {
				if (state->current_menu == MENU_UPGRADES) {
					*upgrade_page = ((*upgrade_page)+1)%6;
				} else if (state->current_menu == MENU_PRINCESS) {
					*princess_page = ((*princess_page)+1)%2;
				}
			}
			bptr = get_button(START_MEDI_B);
			if ( done_button(bptr) ) {
				state->meditate_cooldown = DEFAULT_MEDI_COOLDOWN * get_fps();
				bptr->locked = true;
				state->is_meditating = true;
			}
			bptr = get_button(FACE_EVIL_B);
			if ( done_button(bptr) ) {
				state->current_screen = SCREEN_ENDING;
				state->current_menu = MENU_END_PROMPT1;
			}
			bptr = get_button(QUIT_B);
			if ( done_button(bptr) ) {
				event_confirm_quit(config);
			}
			break;
		case SCREEN_ENDING:
			bptr = get_button(END_OK_B);
			if (done_button(bptr)) {
				if (state->current_menu == MENU_END_FAIL) {
					state->current_screen = SCREEN_GAME_LOOP;
					state->current_menu = MENU_UPGRADES;
					state->magic_count = 0.0;
					Mix_PlayChannel(-1, config->sounds[END_SHOOT_SND], 0);
				} else if (state->current_menu == MENU_END_PROMPT1) {
					++(state->current_menu);
				} else if (state->current_menu == MENU_END_PROMPT2) {
                    state->current_menu = MENU_END_FAIL + check_ending_success(config);
				} else if (state->current_menu == MENU_END_WIN2) {
					++(state->win_count);
					state->current_screen = SCREEN_SAVE;
					state->current_menu = MENU_SV_ENDING;
				}
			}
			break;
		case SCREEN_COMPENDIUM:
			bptr = get_button(COMP_LEFT_B);
			if (done_button(bptr)) {
				--(*upgrade_page);
				if (*upgrade_page >= NUM_COMPENDIUM_ENTRIES) {
					*upgrade_page = NUM_COMPENDIUM_ENTRIES-1;
				}
			}
			bptr = get_button(COMP_RIGHT_B);
			if (done_button(bptr)) {
				*upgrade_page = (*upgrade_page+1) % NUM_COMPENDIUM_ENTRIES;
			}
	}

}

bool check_ending_success(const Config* const config) {
	for (int i = PRINCESS_OFFSET; i < INCANTATION_OFFSET; ++i) {
		if (config->upgrades[i].count < 1) {
			return false;
		}
	}
    return true;
}

void check_unlock_menu_button(const int upgrade_offset, Config* const config) {
	int tab_to_unlock = -1;
	switch (upgrade_offset) {
		case 4:
			tab_to_unlock = 1;
			break;
		case 9:
			tab_to_unlock = 2;
			break;
		case 29:
			tab_to_unlock = 3;
			break;
		case 39:
			tab_to_unlock = 4;
			break;
	}
	if (!tab_to_unlock) {
		return;
	}
	Button* bptr = get_button(MENU_BUTTON_OFFSET+tab_to_unlock);
	unlock_button(bptr);
}

void handle_click_menu_button(const SDL_Point* const mouse_pos, Config* const config) {
	Button* bptr;
	for (int i = 0; i < NUM_MENU_BUTTONS; ++i) {
		bptr = get_button(MENU_BUTTON_OFFSET+i);
		if (bptr->locked) {
			continue;
		}
		if (!clicked_button(bptr,mouse_pos)) {
			continue;
		}
		trigger_menu_button(bptr, config->sounds);
		config->state->current_menu = i;
	}
}

void handle_click_menu_meditate(const SDL_Point* const mouse_pos, Config* const config) {
	Button* bptr = NULL;
	if (config->state->is_meditating) {
		bptr = get_button(MEDI_B);
		if (clicked_button(bptr,mouse_pos)) {
			config->state->meditate_multiplier *= 1.05;
			trigger_button(bptr, config->sounds, config->state);
		}
		return;
	} else {
		if (config->state->meditate_cooldown > 0) {
			return;
		}

		bptr = get_button(START_MEDI_B);
		
		if (!clicked_button(bptr,mouse_pos)) {
			return;
		}

		trigger_button(bptr, config->sounds, config->state);
	}
}

void handle_click_menu_upgrades(const SDL_Point* const mouse_pos, Config* const config, unsigned int* const upgrade_page, unsigned int* const princess_page) {
	Button* buttons = config->buttons;
	Upgrade* upgrades = config->upgrades;
	Mix_Chunk** sounds = config->sounds;
	GameState* state = config->state;

	int upgrade_offset = INCANTATION_OFFSET;
	Button* bptr;
	Upgrade* uptr;
	
	bool (*can_buy)(Upgrade*,GameState*) = can_upgrade;
	int count = 10;
	switch(state->current_menu) {
		case MENU_PRINCESS:
			can_buy = can_princess;
			bptr = get_button(NEXT_B);
			if (clicked_button(bptr,mouse_pos)) {
				trigger_button(bptr, sounds, state);
			}
			upgrade_offset = (*princess_page)*10+PRINCESS_OFFSET;
			if (INCANTATION_OFFSET - upgrade_offset < 10) {
				count = INCANTATION_OFFSET - upgrade_offset;
			}
			break;
		case MENU_UPGRADES:
			bptr = get_button(NEXT_B);
			if (clicked_button(bptr,mouse_pos)) {
				trigger_button(bptr, sounds, state);
			}
			upgrade_offset = (*upgrade_page)*10;
			if (PRINCESS_OFFSET - upgrade_offset < 10) {
				count = PRINCESS_OFFSET - upgrade_offset;
			}
			break;
	}
	// check current page of upgrades
	for (int i = 0; i < count; ++i) {
		// trigger buy of upgrade
		uptr = &(upgrades[upgrade_offset+i]);
		if (!can_buy(uptr,state)) {
			continue;
		}
		if (!clicked_upgrade(uptr,mouse_pos)) {
			continue;
		}
		trigger_upgrade(uptr,sounds,state);

		// unlock next upgrade, if there is one
		
		// note: this will technically unlock 1 past the last upgrade
		// but that is just the first princess, so it doesn't matter
		// we need it this way to unlock the unity princess below
		if (upgrade_offset+i >= NUM_UPGRADES) {
			continue;
		}
		uptr = &(upgrades[upgrade_offset+i+1]);
		unlock_upgrade(uptr);

		// unlock next princess, if there is one
		if ((upgrade_offset+i) % 5 != 4) {
			continue;
		}
		uptr = &(upgrades[(upgrade_offset+i)/5+PRINCESS_OFFSET]);
		unlock_upgrade(uptr);

		// unlock menu tabs
		check_unlock_menu_button(upgrade_offset+i, config);
	}

}

void handle_click_menu_face_evil(const SDL_Point* const mouse_pos, Config* const config) {
	Button* bptr = get_button(FACE_EVIL_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
}

void handle_click_save(const SDL_Point* const mouse_pos, Config* const config) {
	Button* bptr = NULL;
	Button* buttons = config->buttons;
	Mix_Chunk** sounds = config->sounds;
	GameState* state = config->state;
	
	if (state->current_menu >= MENU_SV_CNF_OVERWR) {
		int end_offset = 2 + (state->current_menu == MENU_SV_CNF_QUIT);
		for (int i = 0; i < end_offset; ++i) {
			bptr = get_button(SAVE_YES_B+i);
			if (clicked_button(bptr, mouse_pos)) {
				trigger_button(bptr, sounds, state);
			}
		}
	} else {
		for (int i = 0; i < 4; ++i) {
			bptr = get_button(SAVE_0_B+i);
			if ((state->current_menu == MENU_LD_SLOT) && (bptr->locked)) {
				continue;
			}
			if ((state->current_menu == MENU_SV_SLOT) && !i) {
				continue;
			}
			if (clicked_button(bptr, mouse_pos)) {
				trigger_button(bptr, sounds, state);
			}
		}
	}
}

void event_confirm_quit(Config* const config) {
	config->state->current_screen = SCREEN_SAVE;
	config->state->current_menu = MENU_SV_CNF_QUIT;
	Mix_PlayChannel(-1, config->sounds[MENU_BUTTON_SND], 0);
}

void handle_click_mute_quit(const SDL_Point* mouse_pos, Config* const config) {
	// check mute & quit buttons
	Button* bptr = get_button(QUIT_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
	bptr = get_button(MUTE_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
}

void handle_click_options(const SDL_Point* mouse_pos, Config* const config) {
	Button* bptr = NULL;
	for (int i = 0; i < NUM_OPTIONS; ++i) {
		bptr = get_button(OPTIONS_OFFSET+i);
		if (clicked_button(bptr, mouse_pos)) {
			trigger_button(bptr, config->sounds, config->state);
		}
	}
	bptr = get_button(OPT_CONFIRM_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
}

void handle_click_game_loop(const SDL_Point* mouse_pos, Config* const config, unsigned int* const upgrade_page, unsigned int* const princess_page) {
	handle_click_mute_quit(mouse_pos, config);
	if (config->state->current_menu == MENU_MEDITATE) {
		handle_click_menu_meditate(mouse_pos, config);
	} 
	if (config->state->is_meditating) {
		return;
	}
	
	// check menu buttons
	handle_click_menu_button(mouse_pos, config);
	
	Button* bptr = NULL;
	

	
	// magic button
	bptr = get_button(MAGIC_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
		config->state->magic_count += config->state->magic_per_click; // for magic button, don't wait for button timer
	}
	
	if (config->state->current_menu == MENU_FACE_EVIL) {
		handle_click_menu_face_evil(mouse_pos, config);
	} else {
		handle_click_menu_upgrades(mouse_pos, config, upgrade_page, princess_page);
	}
}

void handle_click_title(const SDL_Point* mouse_pos, Config* const config) {
	Button* bptr = NULL;
	bptr = get_button(NEW_GAME_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	} 
	bptr = get_button(LOAD_GAME_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
	bptr = get_button(OPTIONS_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
	bptr = get_button(COMPENDIUM_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
}

void handle_click_compendium(const SDL_Point* mouse_pos, Config* const config) {
	Button* bptr = NULL;
	bptr = get_button(COMP_LEFT_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
	bptr = get_button(COMP_RIGHT_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
	bptr = get_button(COMP_EXIT_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	}
}

void handle_click_ending(const SDL_Point* mouse_pos, Config* const config) {
	if (config->state->current_menu == MENU_END_WIN1) {
		return;
	}
	Button* bptr = get_button(END_OK_B);
	if (clicked_button(bptr, mouse_pos)) {
		trigger_button(bptr, config->sounds, config->state);
	} 
}

void handle_event_sdl(const SDL_Event event, SDL_Point* const mouse_pos, Config* const config,
						bool* const running, unsigned int* const upgrade_page, unsigned int* const princess_page){	
	Button* bptr = NULL;

	switch(event.type) {
		
		case SDL_QUIT:
			if (config->state->current_screen <= SCREEN_SAVE) {
				*running = false;
				return;
			}
			event_confirm_quit(config);
			break;
			
		case SDL_KEYDOWN:
			// disallow repeat (held) keys
			if (event.key.repeat) {
				break;
			}
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				if (config->state->current_screen <= SCREEN_SAVE) {
					*running = false;
					return;
				}
				event_confirm_quit(config);
			} 
			#ifdef DEBUG
			else if (event.key.keysym.sym == SDLK_p) {
				save_game(config, 1);
			} else if (event.key.keysym.sym == SDLK_l) {
				load_save(config, 1);
				config->state->current_screen = SCREEN_GAME_LOOP;
				config->state->current_menu = MENU_UPGRADES;
			} 
			#endif
			else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_5) {
				if ((config->state->is_meditating) || (config->state->current_screen != SCREEN_GAME_LOOP)) {
					break;
				}
				unsigned int menu_b = event.key.keysym.sym - SDLK_1;
				bptr = get_button(MENU_BUTTON_OFFSET+menu_b);
				if (bptr->locked) {
					break;
				}
				config->state->current_menu = menu_b;
				trigger_menu_button(bptr, config->sounds);
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			// Mouse click coords from event handler
			mouse_pos->x = event.motion.x; 
			mouse_pos->y = event.motion.y;

			switch(config->state->current_screen) {
				case SCREEN_TITLE:
					handle_click_title(mouse_pos, config);
					break;
				case SCREEN_OPTIONS:
					handle_click_options(mouse_pos, config);
					break;
				case SCREEN_SAVE:
					handle_click_save(mouse_pos, config);
					break;
				case SCREEN_GAME_LOOP:
					handle_click_game_loop(mouse_pos, config, upgrade_page, princess_page);
					break;
				case SCREEN_ENDING:
					handle_click_ending(mouse_pos, config);
					break;
				case SCREEN_COMPENDIUM:
					handle_click_compendium(mouse_pos, config);
					break;
			}
			break;
	}
}
