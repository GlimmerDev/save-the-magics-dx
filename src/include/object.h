#ifndef MAGICS_OBJECT_H
#define MAGICS_OBJECT_H

#include <stdbool.h>
#include <time.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "enum.h"


typedef struct Shape {
	E_ShapeType type;
	E_ColorIndex color;
	int x;
	int y;
	int w;
	int h;
	int* v;
} Shape;

typedef struct Star {
    float x, y; // Using float for smoother movement
    float speed;
	int size;
	E_ColorIndex color;
} Star;

typedef struct Button {
	const char* name;
	E_ColorIndex color;
	int click_timer;
	int click_length;
	E_ButtonState state;
	E_SoundIndex sound;
	SDL_Rect rect;
	int centerx;
	int centery;
	int text_size;
	E_ColorIndex text_color;
	int text_offset;
	bool locked;
	bool hide_locked_text;
} Button;

typedef struct Upgrade {
	Button* button;
	const char* name;
	const char* description;
	E_UpgradeType upgrade_type;
	E_UpgradeEffect effect;
	double mult;
	double mps;
	unsigned long long cost;
	unsigned int count;
	unsigned int cooldown;
	unsigned int max_cooldown;
} Upgrade;

typedef struct GameState {
	long double magic_count;
	long long magic_per_click;
	long long magic_per_second;
	double magic_multiplier;
	
	double meditate_multiplier;
	unsigned int meditate_cooldown;
	unsigned int meditate_timer;
	
	bool is_meditating;
	bool is_muted;

	unsigned short current_screen;
	unsigned short current_menu;

	unsigned int upgrade_max;
	unsigned int princess_max;	
	
	unsigned int win_count;
} GameState;

typedef struct SaveSlot {
	char* path;
	bool exists;
	unsigned int save_version;
	time_t last_saved;
	unsigned int win_count;
} SaveSlot;

typedef struct EndState {
	int explosion_delay;
	float explosion_radius;
	float explosion_alpha;
	float ship_glow_alpha;
	SDL_Texture* expl_tx1;
	SDL_Texture* expl_tx2;
} EndState;

typedef struct Config {
	GameState* state;
	Upgrade* upgrades;
	Button* buttons;
	Mix_Chunk** sounds;
	EndState* ending_state;
	
	unsigned int save_version;
	SaveSlot* saves;
	
	unsigned int screen_width;
	unsigned int screen_height;
	unsigned int screen_center_x;
	unsigned int screen_center_y;
	double screen_scale;
	E_AspectType aspect;
	double FPS;
	
	unsigned int autosave_interval;

	E_ReloadState reload_state;
	
	SDL_Window* window;
	SDL_Renderer* renderer;
} Config;



Button* init_buttons(const double fps, GameState* state);

Upgrade* init_upgrades(const double fps, GameState* state);

GameState* init_state(const double fps);

int init_sound_paths();
Mix_Chunk** init_sounds();

EndState* init_end_state(const double fps, GameState* state, SDL_Renderer* renderer);

int set_window_icon(SDL_Window* window);


void free_state(GameState* state);
void free_upgrades(Upgrade* upgrades) ;
void free_config_modules(Config* config);

int init_config_modules(Config* config);

Config* init_magics_config(const E_AspectType aspect, const double fps, const int autosave_interval);

int set_upgrade_rects(Upgrade* upgrades, const int n, const int start_offset);

Button* _get_button(Button* cptr, bool set_bptr, const E_ButtonIndex b);
Button* get_button(const E_ButtonIndex b);
Button* get_button_set_bptr(Button* bptr);

Upgrade* _get_upgrade(Upgrade* uptr, bool set_uptr, const int u);
Upgrade* get_upgrade(const int u);
Upgrade* get_upgrade_set_uptr(Upgrade* uptr);

void trigger_button(Button* bptr, Mix_Chunk** sounds, GameState* state);

void trigger_menu_button(Button* bptr, Mix_Chunk** sounds);

void trigger_upgrade(Upgrade* upgrade, Mix_Chunk** sounds, GameState* state);

void unlock_upgrade(Upgrade* upgrade);

void unlock_button(Button* bptr);

bool done_button(Button* bptr);

bool done_upgrade_button(Upgrade* upgrade);

bool can_upgrade(Upgrade* upgrade, GameState* state);

bool can_princess(Upgrade* princess, GameState* state);

#endif
