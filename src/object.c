#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "include/enum.h"
#include "include/util.h"
#include "include/object.h"
#include "include/save.h"
#include "include/upgrade_data.h"
#include "include/compendium_data.h"
#ifndef __ANDROID__
#include "include/logo_data.h"
#endif

extern const char* UPGRADE_DATA[];
extern const char* COMPENDIUM_DATA[];

const int AUTOSAVE_INTERVALS[] = { 0, 1, 2, 5, 10, 30 };

// Sounds
const char* SOUND_FILENAMES[NUM_SOUNDS] = {
    "upgrade.ogg",              // UPGRADE_SND
    "menu_button.ogg",          // MENU_BUTTON_SND
    "meditate.ogg",             // MEDITATE_SND
    "medi_click.ogg",           // MEDI_CLICK_SND
    "incantation.ogg",          // INCANT_SND
    "incantation_ready.ogg",    // INCANT_READY_SND
    "ending_explode_soft.ogg",  // END_EXPLODE_SND
    "ending_shoot_soft.ogg",    // END_SHOOT_SND
    "engage_evil.ogg",          // ENGAGE_EVIL_SND
    "evil_ship.ogg",            // EVIL_SHIP_SND
	"compendium.ogg"			// COMPENDIUM_SND
};

CompendiumEntry* init_compendium() {
	int data_index = 0;
	CompendiumEntry* compendium = (CompendiumEntry*)calloc(NUM_COMPENDIUM_ENTRIES, sizeof(CompendiumEntry));
	for (int i = 0; i < NUM_COMPENDIUM_ENTRIES; ++i) {
		data_index = i*NUM_COMPENDIUM_FIELDS;
		// NAME;TITLE;AGE;PRONOUNS;COLOR;BIO
		// 0   ;  1;   2;  3;      4
		compendium[i].name = COMPENDIUM_DATA[data_index];
		compendium[i].title = COMPENDIUM_DATA[data_index+1];
		compendium[i].age = COMPENDIUM_DATA[data_index+2];
		compendium[i].pronouns = COMPENDIUM_DATA[data_index+3];
		compendium[i].color = atoi(COMPENDIUM_DATA[data_index+4]);
		compendium[i].bio = COMPENDIUM_DATA[data_index+5];
		// compendium[i].color =
		// compendium[i].text_color =
	}
	return compendium;
}

Button* init_buttons(const double fps, GameState* state) {
	Button* buttons = (Button*)malloc(sizeof(Button)*(NUM_MISC_BUTTONS+NUM_MENU_BUTTONS));
	if (!buttons) return NULL;
	
	const char* button_texts[NUM_MISC_BUTTONS+NUM_MENU_BUTTONS] = {
        CLICK_STR,          				// Magic
		"Next Page",       					// Next Page
		NULL,               				// Face evil
		NULL,               				// Meditate (menu)
		"",          					    // Meditate (clicker)
		"New Game",        					// New game 
		"Load Game",       					// Load game
		"Options",							// Main menu - options
		"Compendium",						// Compendium
		"Prev",								// Compendium - Left
		"Next",								// Compendium - Right
		"Exit",								// Compendium - Exit
		"Yes",             					// Save yes
		"No",              					// Save no
		"Cancel",          					// Save cancel
		"X",               					// Quit
		"Mute",            					// Mute
		"OK",              					// Ending OK
		"",									// Save slot 0 (auto)
		"",									// Save slot 1
		"",									// Save slot 2 
		"",									// Save slot 3 
		"Autosave Interval",				// Options - autosave interval
		"Aspect Ratio",						// Options - aspect ratio
		"Framerate",						// Options - FPS
		"Quit On Save",				        // Options - post save behavior
		"Import Classic Save",				// Options - import classic save
		"Confirm",							// Options - confirm
		"Magic Sources",					// Menu button - magic sources
		"Princesses",                       // Menu button - princesses
		"Meditate",                         // Menu button - meditate
		"Incantations",                     // Menu button - incantations
		"Engage Evil",                      // Menu button - engage evil
    };
    const SDL_Rect button_rects[NUM_MISC_BUTTONS+NUM_MENU_BUTTONS] = {
        {screen_center_x()-50, 150, 100, 100},      // Magic
		{screen_center_x()-390, 555, 100, 38},      // Next Page
		{screen_center_x()-390, 319, 779, 228},     // Face evil
		{screen_center_x()-390, 319, 779, 228},     // Meditate (menu)
		{screen_center_x()-50, 150, 100, 100},      // Meditate (clicker)
        {screen_center_x()-100, 290, 200, 60},      // New game
		{screen_center_x()-100, 290+70*1, 200, 60}, // Load game
		{screen_center_x()-100, 290+70*2, 200, 60}, // Main menu - options
		{screen_center_x()-100, 290+70*3, 200, 60}, // Compendium
		{screen_center_x()-250, 290+70*3, 100, 60},	// Compendium - Left
		{screen_center_x()+150, 290+70*3, 100, 60},	// Compendium - Right
		{screen_center_x()-100, 290+70*3, 200, 60},	// Compendium - Exit
		{screen_center_x()-100, 260, 200, 60},      // Save yes
        {screen_center_x()-100, 340, 200, 60},      // Save no
		{screen_center_x()-100, 420, 200, 60},      // Save cancel
		{screen_width()-48, 6, 40, 30},             // Quit
		{screen_width()-134, 6, 74, 30},            // Mute
        {screen_center_x()-100, 340, 200, 60},      // Ending OK
		{screen_center_x()-200, 180, 400, 60},      // Save slot 0 (auto)
		{screen_center_x()-200, 1*90+180, 400, 60}, // Save slot 1
		{screen_center_x()-200, 2*90+180, 400, 60}, // Save slot 2
		{screen_center_x()-200, 3*90+180, 400, 60}, // Save slot 3
		{screen_center_x()-200, 110, 200, 60},      // Options - autosave interval
		{screen_center_x()-200, 1*90+110, 200, 60}, // Options - aspect ratio
		{screen_center_x()-200, 2*90+110, 200, 60}, // Options - FPS
		{screen_center_x()-200, 3*90+110, 200, 60}, // Options - post save behavior
		{screen_center_x()-200, 3*90+110, 400, 60}, // Options - import classic save
		{screen_center_x()-100, 470, 200, 60},      // Options - confirm
		{screen_center_x()-390, 272, 150, 38},      // Menu button - magic sources
		{screen_center_x()-390+157, 272, 150, 38},  // Menu button - princesses
		{screen_center_x()-390+157*2, 272, 150, 38},// Menu button - meditate
		{screen_center_x()-390+157*3, 272, 150, 38},// Menu button - incantations
		{screen_center_x()-390+157*4, 272, 150, 38} // Menu button - engage evil
    };
    const E_ColorIndex button_colors[NUM_MISC_BUTTONS+NUM_MENU_BUTTONS] = {
        B_PURPLE,                           // Magic
		B_MENU,                             // Next Page
		B_DARK,                             // Face evil
		B_GREEN,                            // Meditate (menu)
		B_MENU,                             // Meditate (clicker)
		B_PURPLE,                           // New game 
		B_BLUE,                             // Load game
		B_MENU,								// Options
		B_DARK,								// Compendium
		B_GREEN,							// Compendium - Left
		B_GREEN,                            // Compendium - Right
		B_MENU,                             // Compendium - Exit
        B_GREEN,                            // Save yes
		B_RED,                              // Save no
		B_MENU,                             // Save cancel
		B_RED,                              // Quit
		B_GREEN,                            // Mute
		B_MENU,                             // Ending OK
		B_PURPLE,                           // Save slot 0 (auto)
		B_PURPLE,                           // Save slot 1
		B_PURPLE,                           // Save slot 2
		B_PURPLE,                           // Save slot 3
		B_PURPLE,                           // Options - autosave interval
		B_MENU,                             // Options - aspect ratio
		B_PINK,                             // Options - FPS
		B_LIGHTBLUE,                        // Options - post save behavior
		B_LIGHTBLUE,                        // Options - import classic save
		B_GREEN,                            // Options - confirm
		B_MENU,                             // Menu button - magic sources
		B_MENU,                             // Menu button - princesses
		B_MENU,                             // Menu button - meditate
		B_MENU,                             // Menu button - incantations
		B_MENU                              // Menu button - engage evil
    };
    const E_SoundIndex button_sounds[NUM_MISC_BUTTONS+NUM_MENU_BUTTONS] = {
        -1,                                 // Magic
		-1,                                 // Next Page
		ENGAGE_EVIL_SND,                    // Face evil
		MENU_BUTTON_SND,                    // Meditate (menu)
		MEDI_CLICK_SND,                     // Meditate (clicker)
		MENU_BUTTON_SND,                    // New game 
		MENU_BUTTON_SND,                    // Load game
		MENU_BUTTON_SND,					// Options
		COMPENDIUM_SND,						// Compendium
		MENU_BUTTON_SND,					// Compendium - Left
		MENU_BUTTON_SND,					// Compendium - Right
		ENGAGE_EVIL_SND,					// Compendium - Exit
		MENU_BUTTON_SND,                    // Save yes
		MENU_BUTTON_SND,                    // Save no
		MENU_BUTTON_SND,                    // Save cancel
        -1,                                 // Quit
		-1,                                 // Mute
		MENU_BUTTON_SND,                    // Ending OK
		MENU_BUTTON_SND,                    // Save slot 0 (auto)
		MENU_BUTTON_SND,                    // Save slot 1
		MENU_BUTTON_SND,                    // Save slot 2
		MENU_BUTTON_SND,                    // Save slot 3
		MENU_BUTTON_SND,                    // Options - autosave interval
		MENU_BUTTON_SND,                    // Options - aspect ratio
		MENU_BUTTON_SND,                    // Options - FPS
		MENU_BUTTON_SND,                    // Options - post save behavior
		MENU_BUTTON_SND,                    // Options - import classic save
		MENU_BUTTON_SND,                    // Options - confirm
		MENU_BUTTON_SND,                    // Menu button - magic sources
		MENU_BUTTON_SND,                    // Menu button - princesses
		MENU_BUTTON_SND,                    // Menu button - meditate
		MENU_BUTTON_SND,                    // Menu button - incantations
		MENU_BUTTON_SND                     // Menu button - engage evil
    };
	const int button_text_sizes[NUM_MISC_BUTTONS+NUM_MENU_BUTTONS] = {
		30,									// Magic
		22,                                 // Next Page
		30,                                 // Face evil
		30,                                 // Meditate (menu)
		30,                                 // Meditate (clicker)
		30,                                 // New game 
		30,                                 // Load game
		30,									// Options
		30,									// Compendium
		30,									// Compendium - Left
		30,									// Compendium - Right
		30,									// Compendium - Exit
		30,                                 // Save yes
		30,                                 // Save no
		30,                                 // Save cancel
		30,                                 // Quit
		24,                                 // Mute
		30,                                 // Ending OK
		30,                                 // Save slot 0 (auto)
		30,                                 // Save slot 1
		30,                                 // Save slot 2
		30,                                 // Save slot 3
		24,                                 // Options - autosave interval
		24,                                 // Options - aspect ratio
		24,                                 // Options - FPS
		24,                                 // Options - post save behavior
		24,                                 // Options - import classic save
		24,                                 // Options - confirm
		20,                                 // Menu button - magic sources
		20,                                 // Menu button - princesses
		20,                                 // Menu button - meditate
		20,                                 // Menu button - incantations
		20,                                 // Menu button - engage evil
	};
	const E_ColorIndex button_text_colors[NUM_MISC_BUTTONS+NUM_MENU_BUTTONS] = {
		WHITE,								// Magic
		BLACK,                              // Next Page
		WHITE,                              // Face evil
		WHITE,                              // Meditate (menu)
		BLACK,                              // Meditate (clicker)
		WHITE,                              // New game 
		WHITE,                              // Load game
		BLACK,								// Options
		WHITE,								// Compendium
		BLACK,								// Compendium - Left
		BLACK,								// Compendium - Right
		BLACK,								// Compendium - Exit
		WHITE,                              // Save yes
		WHITE,                              // Save no
		BLACK,                              // Save cancel
		WHITE,                              // Quit
		BLACK,                              // Mute
		BLACK,                              // Ending OK
		WHITE,                              // Save slot 0 (auto)
		WHITE,                              // Save slot 1
		WHITE,                              // Save slot 2
		WHITE,                              // Save slot 3
		WHITE,                              // Options - autosave interval
		BLACK,                              // Options - aspect ratio
		WHITE,                              // Options - FPS
		BLACK,                              // Options - post save behavior
		BLACK,                              // Options - import classic save
		BLACK,                              // Options - confirm
		BLACK,                              // Menu button - magic sources
		BLACK,                              // Menu button - princesses
		BLACK,                              // Menu button - meditate
		BLACK,                              // Menu button - incantations
		BLACK,                              // Menu button - engage evil
	};

    for (int i = 0; i < NUM_MISC_BUTTONS+NUM_MENU_BUTTONS; ++i) {
        buttons[i].name = button_texts[i];
        buttons[i].color = button_colors[i];
        buttons[i].click_timer = 0;
        buttons[i].click_length = fps/12;
        buttons[i].state = STATE_BTN_IDLE;
        buttons[i].sound = button_sounds[i];
		buttons[i].rect = button_rects[i];
		buttons[i].centerx = buttons[i].rect.x + buttons[i].rect.w / 2;
	    buttons[i].centery = buttons[i].rect.y + buttons[i].rect.h / 2;
		buttons[i].text_size = button_text_sizes[i];
		buttons[i].text_color = button_text_colors[i];
		buttons[i].text_offset = buttons[i].text_size/2;
		// lock buttons after "Magic Sources"
		if (i <= NUM_MISC_BUTTONS) {
			buttons[i].locked = false;
		}
		else { 
			buttons[i].locked = true;
		}
    }
	
	// buttons that require special parameters
	buttons[MEDI_B].click_length = fps;
	buttons[START_MEDI_B].locked = true;
	buttons[COMPENDIUM_B].locked = false;
	
	for (int i = 0; i < 4; ++i) {
		buttons[SAVE_0_B+i].hide_locked_text = true;
	}

	return buttons;
}

Upgrade* init_upgrades(const double fps, GameState* state) {
	int data_index;
	
	Upgrade* upgrades = (Upgrade*)calloc(UPGRADES_END_OFFSET, sizeof(Upgrade));
	if (!upgrades) return NULL;

    for (int i = 0; i < UPGRADES_END_OFFSET; ++i) {
    	data_index = i*NUM_UPGRADE_FIELDS;
		// NAME;DESCRIPTION;MPS;MULT;COST;COLOR;EFFECT;TYPE;COOLDOWN
		//   0       1       2    3   4    5      6      7   8
		upgrades[i].button = (Button*)calloc(1, sizeof(Button));
		if (!upgrades[i].button) {
			// free any already loaded buttons
			for (int j = 0; j < i; ++j) {
				free(upgrades[j].button);
			}
			free(upgrades);
			return NULL;
		}
		upgrades[i].button->name = NULL;
		if (i < INCANTATION_OFFSET) {
			upgrades[i].button->sound = UPGRADE_SND;
		} else {
			upgrades[i].button->sound = INCANT_SND;
		}
		upgrades[i].button->color = atoi(UPGRADE_DATA[data_index+5]);
		upgrades[i].button->click_length = fps / 12;
		
		upgrades[i].name = strdup(UPGRADE_DATA[data_index]);
		upgrades[i].description = strdup(UPGRADE_DATA[data_index+1]);
		upgrades[i].mps = atof(UPGRADE_DATA[data_index+2]);
		upgrades[i].mult = atof(UPGRADE_DATA[data_index+3]);
		upgrades[i].cost = atoll(UPGRADE_DATA[data_index+4]);
		upgrades[i].effect = atoi(UPGRADE_DATA[data_index+6]);
		upgrades[i].upgrade_type = atoi(UPGRADE_DATA[data_index+7]);
		upgrades[i].max_cooldown = atoi(UPGRADE_DATA[data_index+8]) * fps;
		upgrades[i].cooldown = upgrades[i].max_cooldown;
		// keep first upgrade, first princess unlocked
		upgrades[i].button->locked = !((i == 0)|(i == PRINCESS_OFFSET));
		upgrades[i].count = 0;
    }	

	return upgrades;
}

GameState* init_state(const double fps) {
	GameState* new_state = (GameState*)calloc(1, sizeof(GameState));
	if (!new_state) {
		return NULL;
	}
	
	new_state->current_screen = SCREEN_TITLE;
	new_state->upgrade_max = 50;
	new_state->princess_max = 20;
	new_state->magic_per_click = 1;
	new_state->magic_multiplier = 1.0f;
	new_state->meditate_multiplier = 1.0f;
	new_state->meditate_timer = DEFAULT_MEDI_TIMER*fps;
	
	#ifdef DEBUG
		new_state->magic_count = 1000000000000000000.0;
		new_state->meditate_cooldown = 5*fps;
	#else
		new_state->magic_count = 0.0;
		new_state->meditate_cooldown = DEFAULT_MEDI_COOLDOWN*fps;
	#endif
	
	return new_state;
}

Mix_Chunk** init_sounds() {
	LOG_D("Allocate sounds array\n");
    Mix_Chunk** sounds = (Mix_Chunk**)calloc(NUM_SOUNDS, sizeof(Mix_Chunk*));
    if (!sounds) return NULL;

	char path_buf[MAX_PATH_LEN] = "";

	LOG_D("Load sounds\n");
    for (int i = 0; i < NUM_SOUNDS; i++) {
		#ifdef __ANDROID__
		android_load_asset_file(SOUND_FILENAMES[i]);
		snprintf(path_buf, MAX_PATH_LEN, "%s", SOUND_FILENAMES[i]);
		#else
		snprintf(path_buf, MAX_PATH_LEN, "res/%s", SOUND_FILENAMES[i]);
		#endif   
		sounds[i] = Mix_LoadWAV(path_buf);    
        if (!sounds[i]) {
            LOG_E("Error: unable to load sound file '%s': %s\n", SOUND_FILENAMES[i], Mix_GetError());
			// free any already loaded sounds
            for (int j = 0; j < i; j++) {
                Mix_FreeChunk(sounds[j]);
            }
            free(sounds);
            return NULL;
        }
    }
    return sounds;
}

SaveSlot* init_save_slots() {
	SaveSlot* saves = (SaveSlot*)calloc(4, sizeof(SaveSlot));
	if (!saves) return NULL;
	
	return saves;
}

EndState* init_end_state(const double fps, GameState* state, SDL_Renderer* renderer) {
	EndState* e = (EndState*)calloc(1, sizeof(EndState));
	if (!e) {
		return NULL;
	}
	
	SDL_Texture* expl_tx1 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screen_width(), screen_height());
	SDL_Texture* expl_tx2 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screen_width(), screen_height());
	SDL_SetTextureBlendMode(expl_tx1, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(expl_tx2, SDL_BLENDMODE_BLEND);
	
	e->explosion_delay = fps*10;
	e->explosion_radius = 2.0f;
	e->explosion_alpha = 255.0f;
	e->ship_glow_alpha = 150.0f;
	e->eye_blink = fps/30-1;
	e->eye_blink_rate = floor(fps/7.5);
	e->expl_tx1 = expl_tx1;
	e->expl_tx2 = expl_tx2;
	
	return e;
}

int set_window_icon(SDL_Window* window) {
#ifndef __MAGICSMOBILE__
    SDL_Surface* icon_surf = SDL_CreateRGBSurfaceFrom(
        (void*)LOGO,            // Pointer to the pixel data
        LOGO_W,                 // Width of the logo
        LOGO_H,                 // Height of the logo
        32,                     // Bits per pixel (ARGB8888 has 32 bits per pixel)
        LOGO_W * 4,             // Pitch (number of bytes in a row of pixel data)
        0x00FF0000,             // Red mask
        0x0000FF00,             // Green mask
        0x000000FF,             // Blue mask
        0xFF000000              // Alpha mask
    );

    if (icon_surf == NULL) {
        LOG_E("Error: Unable to create icon surface: %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetWindowIcon(window, icon_surf);
	SDL_FreeSurface(icon_surf);
#endif
	return 0;
}

Config* init_magics_config(const E_AspectType aspect, const double fps, const int autosave_interval, const bool quitonsave) {
	Config* config = (Config*)calloc(1, sizeof(Config));
	if (!config) return NULL;

#ifdef __MAGICSMOBILE__
	mobile_set_screen_dims(config);
#else
	config->screen_scale = 1.0;
	config->aspect = aspect;
	switch(aspect) {
		case ASPECT_WIDE:
			config->screen_width = SCREEN_WIDTH_W;
			config->screen_height = SCREEN_HEIGHT_W;
			break;
		case ASPECT_CLASSIC:
			config->screen_width = SCREEN_WIDTH_C;
			config->screen_height = SCREEN_HEIGHT_C;
			break;
	}
#endif
	config->FPS = fps;
	config->screen_center_x = config->screen_width/2;
	config->screen_center_y = config->screen_height/2;

	screen_center_set_cptr(config);
	screen_dims_set_cptr(config);

	LOG_D("Creating SDL window\n");
	config->window = SDL_CreateWindow(
		"Save the Magics!!: Deluxe", // window title
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // window pos
        config->screen_width,config->screen_height, // window res
        SDL_WINDOW_SHOWN );
	if (!config->window) {
		LOG_E("Unable to create window\n");
		SDL_DestroyWindow(config->window);
		SDL_Quit();
		return NULL;
	}

	if (set_window_icon(config->window) < 0) {
		LOG_W("Unable to set window icon\n");
	}

	LOG_D("Creating renderer\n");
	config->renderer = SDL_CreateRenderer(config->window, -1, SDL_RENDERER_ACCELERATED);
	if (!config->renderer) {
		LOG_E("Unable to create SDL renderer\n");
		SDL_DestroyRenderer(config->renderer);
    	SDL_DestroyWindow(config->window);
    	SDL_Quit();
    	return NULL;
	}
	//SDL_RenderSetLogicalSize(config->renderer, config->screen_width, config->screen_height);
	//SDL_RenderSetScale(config->renderer, config->screen_scale, config->screen_scale);
	config->save_version = SAVE_VERSION;
	config->autosave_interval = autosave_interval;
	
	config->quitonsave = quitonsave;

	if (init_config_modules(config) < 0) {
		free(config);
		return NULL;
	}

	return config;
}

void free_state(GameState* state) {
	if (!state) return;
	free(state);
}

void free_upgrades(Upgrade* upgrades) {
	if (!upgrades) return;
	for (int i = 0; i < NUM_UPGRADES; ++i) {
		if (upgrades[i].button) {
			free(upgrades[i].button);
		}
	}
	free(upgrades);
}

void free_config_modules(Config* config) {
	//LOG_D("Free state...\n");
	free_state(config->state);
	
	// free sounds
	//LOG_D("Free sounds...\n");
	for (int i = 0; i < NUM_SOUNDS; ++i) {
		if (config->sounds[i]) {
			Mix_FreeChunk(config->sounds[i]);
		}
	}
	free(config->sounds);
	
	// free buttons
	//LOG_D("Free buttons...\n");
	if (config->buttons) {
		free(config->buttons);
	}
	
	// free upgrades
	//LOG_D("Free upgrades...\n");
	free_upgrades(config->upgrades);
	
	// free save data
	//LOG_D("Free save slot data...\n");
	for (int i = 0; i < 4; ++i) {
		if (config->saves[i].path) {
			free(config->saves[i].path);
		}
		if (config->saves[i].display_str) {
			free(config->saves[i].display_str);
		}
	}
	if (config->saves) {
		free(config->saves);
	}
	
	// free ending state
	//LOG_D("Free ending state...\n");
	SDL_DestroyTexture(config->ending_state->expl_tx1);
	SDL_DestroyTexture(config->ending_state->expl_tx2);
	if (config->ending_state) {
		free(config->ending_state);
	}

	// free compendium
	if (config->compendium) {
		free(config->compendium);
	}
}

int init_config_modules(Config* config) {
	LOG_D("State init\n");
	config->state = init_state(config->FPS);
	if (!config->state) {
		LOG_E("Unable to init state\n");
		free(config);
		return -1;
	}
	LOG_D("Upgrade init\n");
	config->upgrades = init_upgrades(config->FPS, config->state);
	if (!config->upgrades) {
		LOG_E("Unable to init upgrades\n");
		free(config->state);
		return -1;
	}
	LOG_D("Button init\n");
	config->buttons = init_buttons(config->FPS, config->state);
	if (!config->buttons) {
		LOG_E("Unable to init buttons\n");
		free(config->state);
		free(config->upgrades);
		return -1;
	}
	LOG_D("Sound init\n");
	config->sounds = init_sounds();
	if (!config->sounds) {
		LOG_E("Unable to init sounds from paths\n");
		free(config->state);
		free(config->upgrades);
		free(config->buttons);
		return -1;
	}
	LOG_D("Save slot init\n");
	config->saves = init_save_slots();
	if (!config->saves) {
		LOG_E("Unable to init save slots\n");
		free(config->state);
		free(config->upgrades);
		free(config->buttons);
		free(config->sounds);
		return -1;
	}
	LOG_D("End state init\n");
	config->ending_state = init_end_state(config->FPS, config->state, config->renderer);
	if (!config->ending_state) {
		LOG_E("Unable to init ending state\n");
		free(config->state);
		free(config->upgrades);
		free(config->buttons);
		free(config->sounds);
		free(config->saves);
		return -1;
	}

	LOG_D("Compendium init\n");
	config->compendium = init_compendium();
	if (!config->compendium) {
		LOG_E("Unable to init compendium\n");
		free(config->state);
		free(config->upgrades);
		free(config->buttons);
		free(config->sounds);
		free(config->saves);
		free(config->ending_state);
		return -1;
	}

	LOG_D("Save path init\n");
	char save_path_buf[MAX_PATH_LEN] = "";
	for (int i = 0; i < 4; ++i) {
		snprintf(save_path_buf, MAX_PATH_LEN, "%s/magics_save_%d.json", get_save_path(), i);
		config->saves[i].path = strdup(save_path_buf);
		if (!(config->saves[i].path)) {
			LOG_E("Unable to init save paths\n");
			free(config->state);
			free(config->upgrades);
			free(config->buttons);
			free(config->sounds);
			free(config->saves);
			free(config->ending_state);
			free(config->compendium);
			return -1;
		}
	}

	return 0;
}

/*
 * Initializes the upgrade button shapes to align them into the 5x2 grid pattern
 * used in the game. Saves time by pre-computing what page they fall on, and which
 * slot on the page to place them in.
 */
int set_upgrade_rects(Upgrade* upgrades, const int n, const int start_offset) {
	for (int i = 0; i < n; i+=10) {
		for (int j = i; j < i+10; ++j) {
			if (j >= UPGRADES_END_OFFSET) break;
			if (j >= n) break;
			// Set up rect depending on slot
			Button* bptr = upgrades[j+start_offset].button;
			SDL_Rect* rect = &(bptr->rect);
			rect->x = ((j-i)%5)*157+screen_center_x()-390;
			rect->y = ((j-i)/5*117)+319;
			rect->w = 150;
			rect->h = 110;
			bptr->centerx = rect->x + rect->w / 2;
			bptr->centery = rect->y + rect->h / 2;
		}
	}
	return 0;
}

/*
 * Gets a button by its index, using a static pointer to the buttons array that can
 * be assigned during initialzation. Use the "get_button" and "get_button_set_bptr"
 * helper functions instead of calling this directly.
 */
Button* _get_button(Button* bptr, bool set_bptr, const E_ButtonIndex b) {
	static Button* buttons = NULL;
	if (set_bptr) {
		buttons = bptr;
		return NULL;
	} else if (!buttons) {
		return NULL;
	}
	return &(buttons[b]);
}

/*
 * Gets a button by index, assuming the bptr has been registered using "get_button_set_bptr".
 */
Button* get_button(const E_ButtonIndex b) {
	return _get_button(NULL, false, b);
}

/*
 * Registers the button array to the _get_button function, for use with "get_button".
 */
Button* get_button_set_bptr(Button* bptr) {
	return _get_button(bptr, true, -1);
}

/*
 * Gets an upgrade by its index, using a static pointer to the upgrades array that can
 * be assigned during initialzation. Use the "get_upgrade" and "get_upgrade_set_uptr"
 * helper functions instead of calling this directly.
 */
Upgrade* _get_upgrade(Upgrade* uptr, bool set_uptr, const int u) {
	static Upgrade* upgrades = NULL;
	if (set_uptr) {
		upgrades = uptr;
		return NULL;
	} else if (!upgrades) {
		return NULL;
	}
	return &(upgrades[u]);
}

/*
 * Gets an upgrade by index, assuming the uptr has been registered using "get_upgrade_set_uptr".
 */
Upgrade* get_upgrade(const int u) {
	return _get_upgrade(NULL, false, u);
}

/*
 * Registers the upgrades array to the _get_upgrade function, for use with "get_upgrade".
 */
Upgrade* get_upgrade_set_uptr(Upgrade* uptr) {
	return _get_upgrade(uptr, true, -1);
}

void trigger_button(Button* bptr, Mix_Chunk** sounds, GameState* state) {
	bptr->click_timer = bptr->click_length * get_fps();
	bptr->state = STATE_BTN_CLICKED;
	
	if (bptr->sound != -1) {
		Mix_PlayChannel(-1, sounds[bptr->sound], 0);
	}
}

void trigger_menu_button(Button* bptr, Mix_Chunk** sounds) {
	if (bptr->sound != -1) {
		Mix_PlayChannel(-1, sounds[bptr->sound], 0);
	}
} 

void trigger_upgrade(Upgrade* upgrade, Mix_Chunk** sounds, GameState* state) {
	trigger_button(upgrade->button, sounds, state);
	state->magic_count -= upgrade->cost;
	if (upgrade->upgrade_type != UPGRADE_TYPE_INCANTATION) {
		++upgrade->count;
	}
	upgrade->cost = round(upgrade->cost*1.18f);
	
	// do effect
	switch (upgrade->effect) {
		case EFFECT_INCREASE_MPS:
			state->magic_per_second += upgrade->mps;
			break;
		case EFFECT_MULT_ADD:
			state->magic_multiplier += upgrade->mult;
			break;
		case EFFECT_MULT_MULT:
			state->magic_multiplier *= upgrade->mult;
			break;
		case EFFECT_CLICK_ADD:
			state->magic_per_click += upgrade->mps;
			break;
		case EFFECT_CLICK_MULT:
			state->magic_per_click *= upgrade->mult;
			break;
		case EFFECT_TOTAL_MULT:
			state->magic_count *= upgrade->mult;
			break;
		case EFFECT_REDUCE_MEDI_COOLDOWN:
			state->meditate_cooldown = get_fps();
			break;
		case EFFECT_MULT_MEDI_TIMER:
			state->meditate_timer = DEFAULT_MEDI_TIMER * get_fps() * upgrade->mult;
			break;
		case EFFECT_SET_MEDI_MULT:
			state->meditate_multiplier = upgrade->mult;
			break;
		case EFFECT_INCREASE_MAX_UPGR:
			if (state->upgrade_max < 250) {
				state->upgrade_max += 10;
			}
			break;
		case EFFECT_INCREASE_MAX_PRIN:
			if (state->princess_max < 100) {
				state->princess_max += 10;
			}
			break;
		case EFFECT_DIV_INCANT_TIMERS:
			for (int i = INCANTATION_OFFSET; i < UPGRADES_END_OFFSET; ++i) {
				get_upgrade(i)->cooldown /= upgrade->mult;
			}
			break;
		default:
			LOG_W("Unknown effect: %d\n", upgrade->effect);
			break;
	}

	if (upgrade->upgrade_type != UPGRADE_TYPE_INCANTATION) {
		return;
	}
	upgrade->cooldown = upgrade->max_cooldown;
	upgrade->button->locked = true;
}

void unlock_upgrade(Upgrade* upgrade) {
	upgrade->button->locked = 0;
}

void unlock_button(Button*  bptr) {
	bptr->locked = 0;
}

bool done_button(Button* bptr) {
	if ((bptr->state == STATE_BTN_IDLE) || (bptr->click_timer > 0)) {
		return false;
	}
	return true;
}

bool done_upgrade_button(Upgrade* upgrade) {
	return done_button(upgrade->button);
}

bool can_upgrade(Upgrade* upgrade, GameState* state) {
	bool res;
	res = (state->magic_count >= upgrade->cost) &
			(!upgrade->button->locked) &
				(upgrade->count < state->upgrade_max);
	return res;
}

bool can_princess(Upgrade* princess, GameState* state) {
	bool res;
	res = (state->magic_count >= princess->cost) &
			(!princess->button->locked) &
				(princess->count < state->princess_max);
	return res;	
}
