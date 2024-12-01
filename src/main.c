#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_FontCache.h>
#include "include/enum.h"
#include "include/draw.h"
#include "include/object.h"
#include "include/util.h"
#include "include/event.h"
#include "include/save.h"

// GLOBAL VARIABLES

// ********************
// FUNCTION DEFINITIONS
// ********************

void _magics_cleanup(Config* const cptr, bool set_cptr, bool quit_sdl) {
	static Config* config = NULL;
	if (set_cptr) {
		config = cptr;
		return;
	} else if (!config) {
		return;
	}
	
	// quit SQL
	LOG_D("Destroying renderer...\n");
	SDL_DestroyRenderer(config->renderer);
	LOG_D("Destroying window...\n");
    SDL_DestroyWindow(config->window);
	if (quit_sdl) {
		SDL_Quit();
	}
	// cleanup misc memory
	LOG_D("Clean up fonts...\n");
	magics_font_cleanup();
	LOG_D("Clean up shape vertices...\n");
	bg_vertex_cleanup();
	LOG_D("Clean up config modules...\n");
	free_config_modules(config);
	free(config);
	LOG_D("Clean up complete!\n");
	return;
}

void magics_cleanup() {
	_magics_cleanup(NULL, false, true);
}

void magics_cleanup_set_cptr(Config* const cptr) {
	_magics_cleanup(cptr, true, false);
}

void magics_register_config(Config* const cptr) {
	magics_cleanup_set_cptr(cptr);
	get_button_set_bptr(cptr->buttons);
	get_upgrade_set_uptr(cptr->upgrades);
	draw_button_set_ptrs(cptr);
	get_fps_set_cptr(cptr);
}

/*
 * Partially reloads the game by specifically re-initializing the state and
 * upgrade components of the config. Useful on mobile when we need to return
 * to title without quitting.
 */
int magics_soft_reload(Config* config) {
	free_state(config->state);
	free_upgrades(config->upgrades);
	LOG_D("Init new state\n");
	config->state = init_state(config->FPS);
	if (!config->state) {
		LOG_E("Error getting new state\n");
		return -1;
	}
	LOG_D("Init new upgrades\n");
	config->upgrades = init_upgrades(config->FPS, config->state);
	if (!config->upgrades) {
		LOG_E("Error getting new upgrades\n");
		return -1;
	}
	LOG_D("Re-lock menu buttons\n");
	for (int i = MENU_PRINCESS_B; i <= MENU_EVIL_B; ++i) {
		config->buttons[i].locked = true;
	}
	set_upgrade_rects(config->upgrades, NUM_UPGRADES, 0);
	set_upgrade_rects(config->upgrades, NUM_PRINCESSES, PRINCESS_OFFSET);
	set_upgrade_rects(config->upgrades, NUM_INCANTATIONS, INCANTATION_OFFSET);
	return 0;
}

/*
 * Completely reloads the game by cleaning up everything (as if exiting),
 * and re-initializing. Useful for when calculation-dependent things like
 * resolution and FPS are changed.
 */
Config* magics_hard_reload(E_AspectType aspect, double fps, int autosave_interval, float* time_step, float* max_accumulator) {
	// perform cleanup, but without quitting SDL
	_magics_cleanup(NULL, false, false);
	
	// get a new config
	Config* new_config = init_magics_config(aspect, fps, autosave_interval);
	if (!new_config) {
		LOG_E("Error: could not create reloaded config\n");
		return NULL;
	}
	magics_register_config(new_config);
    
	init_fonts(new_config->renderer);
	init_shapes(new_config);
	
	// compensate for new FPS value
	(*time_step) = 1.0f / new_config->FPS;
	(*max_accumulator) = (*time_step) * MAX_LAG_FRAMES;
	
	return new_config;
}

/*
 * Checks if a hard reload was requested via the config's reload_state.
 * If so, return true.
 */
bool check_do_reload(Config* config){
	return (config->reload_state == RELOAD_STATE_EXECUTE);
}

/*
 * Some platforms require main, while others require SDL_main.
 * This should ensure that all platforms are happy.
 */
#if !defined(__MAGICSMOBILE__) && !defined(main)
int main(int argc, char *argv[]) {
	return SDL_main(argc, argv);
}
#endif

int SDL_main(int argc, char *argv[]) {
	// Init SDL & SDL modules
	LOG_D("Initializing SDL...\n");
	SDL_Init(SDL_INIT_VIDEO);
	LOG_D("Initializing SDL_mixer...\n");
    Mix_Init(MIX_INIT_OGG);
	if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) == -1 ) { 
		LOG_E("Error: could not initialize mixer\n");
		SDL_Quit();
		return -1; 
	}

	// Seed random
	srand(time(NULL));

	LOG_D("Checking for config file...\n");
	if (!config_file_exists()) {
		create_default_config_file();
	}
	LOG_D("Initializing config...\n");
	Config* config = load_config_from_file();
	if (!config) {
		LOG_E("Error: could not load magics config\n");
    	return -1;
	}
	// Once we have a config, make sure we clean up at exit
	LOG_D("Registering config ptr in convenience functions...\n");
	magics_register_config(config);
	atexit(magics_cleanup);

	create_save_path();
	
	LOG_D("Initializing fonts...\n");
	init_fonts(config->renderer);
	
	LOG_D("Initializing shapes...\n");
	init_shapes(config);

	// framerate related
	unsigned int prev_ticks = SDL_GetTicks();
	unsigned int fps_prev_ticks = prev_ticks;
	float accumulator = 0.0f;
	int avg_fps = 0;
	float TIME_STEP = 1.0f / get_fps();
	int frame_count = 0;
	char fps_buf[16] = "0";
	float MAX_ACCUMULATOR = TIME_STEP * MAX_LAG_FRAMES;

	const char* version_string = "v" VERSION_NUMBER " by @glimmer@chaosfem.tw";
	
	SDL_Point mouse_pos;
	
	unsigned int upgrade_page = 0;
	unsigned int princess_page = 0;
	unsigned int autosave_timer = 0;
	unsigned int save_slot = 0;
	
	Button* bptr = NULL;
	
	bool running = true;

	LOG_D("Starting main loop.\n");
	
	while (running) {
		if (check_do_reload(config)) {
			LOG_D("***\nHard reloading...\n");
			config = magics_hard_reload(config->aspect, get_fps(), config->autosave_interval, 
										&TIME_STEP, &MAX_ACCUMULATOR);
			LOG_D("***\nHard reload complete.\n");
		}

		unsigned int curr_ticks = SDL_GetTicks();
		unsigned int frame_ticks = curr_ticks - prev_ticks;
		prev_ticks = curr_ticks;
		
		float delta_time = frame_ticks / 1000.0f;
		
		// limit lag frames
		if (delta_time > MAX_ACCUMULATOR) {
			delta_time = MAX_ACCUMULATOR;
		}
		
		accumulator += delta_time;	
		
		while (accumulator >= TIME_STEP) {
			// *********************
			// *** HANDLE EVENTS ***
			// *********************
			
			// SDL events
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				handle_event_sdl(event, &mouse_pos, config, &running, &upgrade_page, &princess_page);
			}
			
			// update timers
			update_button_timers(config->buttons, config->upgrades, config->state);
			update_ending_timers(config);
			if (config->state->current_screen == SCREEN_GAME_LOOP) {
				// if incantations unlocked, update their timers
				// per classic behavior, this applies even when meditating
				if (!(get_button(MENU_INCANT_B)->locked)) {
					update_incantation_timers(config->upgrades, config->sounds);
				}
				update_autosave_timer(config, &autosave_timer);
				// if meditating, update meditation time left
				if (config->state->is_meditating) {
					update_meditate_timer(config->state);
					// bonus multiplier will decrease at 1/10 speed
					if (config->state->meditate_multiplier > 1.0) {
						config->state->meditate_multiplier *= (1.0 - ((0.0005 * 30) / get_fps()));
					}
				}
				// else, if meditation is unlocked, update meditate cooldown
				else if (!(get_button(MENU_MEDI_B))->locked) {
					update_meditate_cooldown(config->state, config->sounds);
					// decrease bonus multiplier by regular rate
					if (config->state->meditate_multiplier > 1.0) {
						config->state->meditate_multiplier *= (1.0 - ((0.005 * 30) / get_fps()));
					}
				}
				// highlight currently selected menu button
				for (int i = 0; i < NUM_MENU_BUTTONS; ++i) {
					if (config->state->current_menu == i) {
						config->buttons[MENU_BUTTON_OFFSET+i].color = B_MENU+1;
					}
					else {
						config->buttons[MENU_BUTTON_OFFSET+i].color = B_MENU;
					}
				}
				
				// update magics count
				config->state->magic_count += (config->state->magic_per_second * 
											config->state->magic_multiplier * config->state->meditate_multiplier / get_fps());
			}
			
			// update starfield
			update_starfield(config->state);

			// check buttons that are ready to trigger their actions
			check_done_buttons(config, &upgrade_page, &princess_page, &running, &save_slot);
			
			// *************************
			// *** END HANDLE EVENTS ***
			// *************************
			
			// *************************
			// *** START DRAW ***
			// *************************
		
			clear_screen(config->renderer, BACKGROUND);
			
			// draw planet, meditation, or ending background
			draw_background(config);
			
			// Draw FPS and version info
			draw_text(version_string, config->screen_width-140, config->screen_height-24, FONT_RPG, 20, WHITE, config->renderer);
			draw_text(fps_buf, 18, 2, FONT_RPG, 18, PLANET3, config->renderer);
			
			switch(config->state->current_screen) {
				case SCREEN_TITLE:
					draw_screen_title(config);
					break;
				case SCREEN_OPTIONS:
					draw_screen_options(config);
					break;
				case SCREEN_SAVE:
					draw_screen_save(config);
					break;
				case SCREEN_GAME_LOOP:
					draw_screen_game_loop(config, &upgrade_page, &princess_page);
					break;
				case SCREEN_ENDING:
					draw_screen_ending(config->renderer, config);
					break;
			}
			
			// ****************
			// *** END DRAW ***
			// ****************
			
			// Update the screen
			SDL_RenderPresent(config->renderer);
		
			accumulator -= TIME_STEP;
			++frame_count;
		}

		// recalculate FPS
		if (curr_ticks - fps_prev_ticks >= 1000) {
			avg_fps = frame_count / ((curr_ticks - fps_prev_ticks) / 1000.0f);
			snprintf(fps_buf, 16, "%d", avg_fps);
			frame_count = 0;
			fps_prev_ticks = curr_ticks;
		}

		// On mobile, we must avoid quitting, as this is considered bad practice.
		// Instead we "soft reload", and return to title screen.
		#ifdef __MAGICSMOBILE__
		if (!running) {
			running = true;
			LOG_D("***\nSoft reloading...\n");
			if (magics_soft_reload(config) < 0) {
				LOG_E("Error during main loop soft reload!\n");
				return -1;
			}
			LOG_D("***\nSoft reload complete.\n");
		}
		#endif
	}

    return 0;
}
