#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "include/save.h"
#include "include/event.h"
#include "include/util.h"

void print_json(json_t *root) { print_json_aux(root, 0); }

void print_json_aux(json_t *element, int indent) {
    switch (json_typeof(element)) {
        case JSON_OBJECT:
            print_json_object(element, indent);
            break;
        case JSON_ARRAY:
            print_json_array(element, indent);
            break;
        case JSON_STRING:
            print_json_string(element, indent);
            break;
        case JSON_INTEGER:
            print_json_integer(element, indent);
            break;
        case JSON_REAL:
            print_json_real(element, indent);
            break;
        case JSON_TRUE:
            print_json_true(element, indent);
            break;
        case JSON_FALSE:
            print_json_false(element, indent);
            break;
        case JSON_NULL:
            print_json_null(element, indent);
            break;
        default:
            LOG_E("unrecognized JSON type %d\n", json_typeof(element));
    }
}

void print_json_indent(int indent) {
    int i;
    for (i = 0; i < indent; i++) {
        putchar(' ');
    }
}

const char *json_plural(size_t count) { return count == 1 ? "" : "s"; }

void print_json_object(json_t *element, int indent) {
    size_t size;
    const char *key;
    json_t *value;

    print_json_indent(indent);
    size = json_object_size(element);

    LOG_I("JSON Object of %lld pair%s:\n", (long long)size, json_plural(size));
    json_object_foreach(element, key, value) {
        print_json_indent(indent + 2);
        LOG_I("JSON Key: \"%s\"\n", key);
        print_json_aux(value, indent + 2);
    }
}

void print_json_array(json_t *element, int indent) {
    size_t i;
    size_t size = json_array_size(element);
    print_json_indent(indent);

    LOG_I("JSON Array of %lld element%s:\n", (long long)size, json_plural(size));
    for (i = 0; i < size; i++) {
        print_json_aux(json_array_get(element, i), indent + 2);
    }
}

void print_json_string(json_t *element, int indent) {
    print_json_indent(indent);
    LOG_I("JSON String: \"%s\"\n", json_string_value(element));
}

void print_json_integer(json_t *element, int indent) {
    print_json_indent(indent);
    LOG_I("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void print_json_real(json_t *element, int indent) {
    print_json_indent(indent);
    LOG_I("JSON Real: %f\n", json_real_value(element));
}

void print_json_true(json_t *element, int indent) {
    (void)element;
    print_json_indent(indent);
    LOG_I("JSON True\n");
}

void print_json_false(json_t *element, int indent) {
    (void)element;
    print_json_indent(indent);
    LOG_I("JSON False\n");
}

void print_json_null(json_t *element, int indent) {
    (void)element;
    print_json_indent(indent);
    LOG_I("JSON Null\n");
}

/*
 * Parse text into a JSON object. If text is valid JSON, returns a
 * json_t structure, otherwise prints and error and returns null.
 */
json_t *load_json(const char *text) {
    json_t *root;
    json_error_t error;

    root = json_loads(text, 0, &error);

    if (root) {
        return root;
    } else {
        LOG_E("json error on line %d: %s\n", error.line, error.text);
        return (json_t *)0;
    }
}

json_t* create_save_json_upgrades(Config* config) {
	json_t* j_upgrades = json_object();
	if (!j_upgrades) {
		LOG_E("Error creating j_upgrades object\n");
		return NULL;
	}
	json_t* j_upgrade_counts = json_array();
	if (!j_upgrade_counts) {
		LOG_E("Error creating j_upgrade_counts object\n");
		return NULL;
	}
	Upgrade* upgrades = config->upgrades;
	
	for (int i = 0; i < INCANTATION_OFFSET; ++i) {
		json_array_append(j_upgrade_counts, json_integer(upgrades[i].count));
	}
	for (int i = INCANTATION_OFFSET; i < UPGRADES_END_OFFSET; ++i) {
		json_array_append(j_upgrade_counts, json_integer(upgrades[i].cooldown));
	}
	
	json_object_set_new(j_upgrades, "counts", j_upgrade_counts);
	
	// calculate unlocks
	unsigned int upgrade_unlocked, princess_unlocked = 0;
	int i = 0;
	while (i < PRINCESS_OFFSET) {
		if (upgrades[i].button->locked) break;
		++i;
	}
	upgrade_unlocked = i;
	i = PRINCESS_OFFSET;
	while (i < INCANTATION_OFFSET) {
		if (upgrades[i].button->locked) break;
		++i;
	}
	princess_unlocked = i - PRINCESS_OFFSET;
	i = 0;
	
	json_object_set_new(j_upgrades, "upgrade_unlocked", json_integer(upgrade_unlocked));
	json_object_set_new(j_upgrades, "princess_unlocked", json_integer(princess_unlocked));
	
	return j_upgrades;
}

json_t* create_save_json_state(Config* config) {
	json_t* j_state = json_object();
	if (!j_state) {
		LOG_E("Error creating j_state object\n");
		return NULL;
	}
	GameState* state = config->state;
	
	json_object_set_new(j_state, "is_muted", json_integer(state->is_muted));
	json_object_set_new(j_state, "magic_count", json_real(state->magic_count));
	json_object_set_new(j_state, "magic_per_click", json_integer(state->magic_per_click));
	json_object_set_new(j_state, "magic_per_second", json_integer(state->magic_per_second));
	json_object_set_new(j_state, "magic_multiplier", json_real(state->magic_multiplier));
	json_object_set_new(j_state, "meditate_cooldown", json_integer(state->meditate_cooldown));
	json_object_set_new(j_state, "meditate_timer", json_integer(state->meditate_timer));
	json_object_set_new(j_state, "upgrade_max", json_integer(state->upgrade_max));
	json_object_set_new(j_state, "princess_max", json_integer(state->princess_max));	
	json_object_set_new(j_state, "win_count", json_integer(state->win_count));
	return j_state;
}

json_t* create_save_json_buttons(Config* config) {
	Button* buttons = config->buttons;
	json_t* j_buttons = json_object();
	if (!j_buttons) {
		LOG_E("Error creating j_buttons object\n");
		return NULL;
	}
	
	// calculate menu button unlocks
	int i = MENU_BUTTON_OFFSET;
	while (i < NUM_BUTTONS) {
		if (buttons[i].locked) break;
		++i;
	}
	int menu_unlocked = i - MENU_BUTTON_OFFSET;
	json_object_set_new(j_buttons, "menu_button_unlocked", json_integer(menu_unlocked));
	
	return j_buttons;
}

json_t* create_save_json(Config* config) {
	json_t* j_root = json_object();
	if (!j_root) {
		LOG_E("Error creating j_root object\n");
		return NULL;
	}
	
	json_t* j_state = create_save_json_state(config);
	json_t* j_buttons = create_save_json_buttons(config);
	json_t* j_upgrades = create_save_json_upgrades(config);
	if (!j_state || !j_upgrades || !j_buttons) {
		return NULL;
	}
	
	json_object_set_new(j_root, "state", j_state);
	json_object_set_new(j_root, "buttons", j_buttons);
	json_object_set_new(j_root, "upgrades", j_upgrades);
	
	long timestamp = (long)time(NULL);
	unsigned int magic_missile = calc_magic_missile(timestamp, config);
	
	json_object_set_new(j_root, "save_version", json_integer(config->save_version));
	json_object_set_new(j_root, "last_saved", json_integer(timestamp));
	json_object_set_new(j_root, "magic_missile", json_integer(magic_missile));
	
	return j_root;
}

int save_game(Config* config, const unsigned short slot) {
	if (!config->saves[slot].path) {
		return -1;
	}
	json_t* save_data = create_save_json(config);
	if (!save_data) {
		LOG_E("Error creating save_data object for save\n");
		return -1;
	}
	
	if (write_json_file(save_data, config->saves[slot].path) < 0) {
		LOG_E("Error writing save file: %s\n", config->saves[slot].path);
		json_decref(save_data);
		return -1;
	} else {
		#ifdef DEBUG
		LOG_D("Wrote save file to: %s\n", config->saves[slot].path);
		#endif
	}
	
	json_decref(save_data);
	
	return 0;
}

int load_save_json_upgrades(json_t* save_data, Config* config, const bool is_classic) {
	Upgrade* upgrades = config->upgrades;
	
	json_t* j_upgrades = json_object_get(save_data, "upgrades");
	json_t* j_upgrade_counts = json_object_get(j_upgrades, "counts");
	
	if (is_classic) {
		// compensate for less princesses
		for (int i = 50; i < 55; ++i) {
			json_array_insert_new(j_upgrade_counts, i, json_integer(0));
		}
		json_array_insert_new(j_upgrade_counts, 64, json_integer(0));
	}
	if (json_array_size(j_upgrade_counts) != UPGRADES_END_OFFSET) {
		LOG_E("Error: Upgrade count array size not equal to save data\n");
		return -1;
	}
	
	// load upgrade levels
	for (int i = 0; i < INCANTATION_OFFSET; ++i) {
		upgrades[i].count = json_integer_value(json_array_get(j_upgrade_counts, i));
		// reset & recalculate upgrade cost
		upgrades[i].cost = atoll(UPGRADE_DATA[i*NUM_UPGRADE_FIELDS+4]);
		for (int j = 0; j < upgrades[i].count; ++j) {
			upgrades[i].cost = round(upgrades[i].cost*1.18f);
		}
	} for (int i = INCANTATION_OFFSET; i < UPGRADES_END_OFFSET; ++i) {
		upgrades[i].cooldown = json_integer_value(json_array_get(j_upgrade_counts, i));
	}
	
	// unlock upgrades/princesses
	unsigned int upgrade_unlocked = json_integer_value(json_object_get(j_upgrades, "upgrade_unlocked"));
	unsigned int princess_unlocked = json_integer_value(json_object_get(j_upgrades, "princess_unlocked"));
	for (int i = 0; i < PRINCESS_OFFSET; ++i) {
		upgrades[i].button->locked = (i >= upgrade_unlocked);
	}
	for (int i = PRINCESS_OFFSET; i < INCANTATION_OFFSET; ++i) {
		upgrades[i].button->locked = ((i-PRINCESS_OFFSET) >= princess_unlocked);
	}
	
	return 0;
}

void load_save_json_state(json_t* save_data, Config* config) {
	GameState* state = config->state;
	
	json_t* j_state = json_object_get(save_data, "state");
	
	state->is_muted = json_integer_value(json_object_get(j_state, "is_muted"));
	event_update_mute(config);
	// TODO: if FPS is different than the save's FPS, recalculate values as needed
	// (for now, the FPS is always tied to the save file)
	state->magic_count = json_real_value(json_object_get(j_state, "magic_count"));
	state->magic_per_click = json_integer_value(json_object_get(j_state, "magic_per_click"));
	state->magic_per_second = json_integer_value(json_object_get(j_state, "magic_per_second"));
	state->magic_multiplier = json_real_value(json_object_get(j_state, "magic_multiplier"));
	state->meditate_cooldown = json_integer_value(json_object_get(j_state, "meditate_cooldown"));
	state->meditate_timer = json_integer_value(json_object_get(j_state, "meditate_timer"));
	state->upgrade_max = json_integer_value(json_object_get(j_state, "upgrade_max"));
	state->princess_max = json_integer_value(json_object_get(j_state, "princess_max"));
	state->win_count = json_integer_value(json_object_get(j_state, "win_count"));
	
	json_decref(j_state);
}

void load_save_json_buttons(json_t* save_data, Config* config) {
	Button* buttons = config->buttons;
	
	json_t* j_buttons = json_object_get(save_data, "buttons");
	int menu_button_unlocked = json_integer_value(json_object_get(j_buttons, "menu_button_unlocked"));
	
	for (int i = 0; i < 5; ++i) {
		buttons[MENU_BUTTON_OFFSET+i].locked = (i >= menu_button_unlocked);
	}
	json_decref(j_buttons);
}

json_t* load_save_json(const char* path) {	
	json_error_t error;
	json_t* save_data = load_json_file(path, &error);
	json_incref(save_data);
	if (!save_data) {
		LOG_E("Error creating save_data object for load\n");
		return (json_t*)NULL;
	}
	return save_data;
}

char* get_base_path() {
	static char base_path[MAX_PATH_LEN] = "";
	if (strcmp(base_path, "") == 0) {
		strcpy(base_path, SDL_GetPrefPath(MAGICS_ORG_STR, MAGICS_APP_STR));
		LOG_D("Set base path to: %s", base_path);
	}
	return base_path;
}

char* get_config_file_path() {
	static char config_path[MAX_PATH_LEN] = "";
	if (strcmp(config_path, "") == 0) {
		snprintf(config_path, MAX_PATH_LEN, "%s%s", get_base_path(), "config.json");
		LOG_D("Set config path to: %s", config_path);
	}
	return config_path;
}

char* get_save_path() {
	static char save_path[MAX_PATH_LEN] = "";
	if (strcmp(save_path, "") == 0) {
		snprintf(save_path, MAX_PATH_LEN, "%s%s", get_base_path(), "save");
		LOG_D("Set save path to: %s", save_path);
	}
	return save_path;
}

char* create_save_path() {
	const char* config_path = get_base_path();
	char* save_path = get_save_path();
	if (!file_exists(config_path)) {
		if (create_directory(config_path) < 0) {
			return NULL;
		}
	}
	if (!file_exists(save_path)) {
		if (create_directory(save_path) < 0) {
			return NULL;
		}
	}

	return save_path;
}

void load_save_properties(Config* config, const unsigned short slot) {
	if (!config->saves[slot].path) {
		return;
	}
	
	config->saves[slot].exists = file_exists(config->saves[slot].path);
	config->buttons[SAVE_0_B+slot].locked = !config->saves[slot].exists;
	if (!(config->saves[slot].exists)){
		return;
	}
	
	json_t* save_data = load_save_json(config->saves[slot].path);
	json_t* j_state = json_object_get(save_data, "state");
	
	config->saves[slot].last_saved = json_integer_value(json_object_get(save_data, "last_saved"));
	config->saves[slot].win_count = json_integer_value(json_object_get(j_state, "win_count"));
	
	// indicate completed save games
	if (config->saves[slot].win_count > 0) {
		config->buttons[SAVE_0_B+slot].color = B_SAVE_CLEAR;
	}
	
	json_decref(save_data);
}

void load_save(Config* config, const unsigned short slot) {
	if (!config->saves[slot].path) {
		return;
	}
	
	json_t* save_data = load_save_json(config->saves[slot].path);
	if (!save_data) {
		exit(ERR_JSON_LOAD);
	}
	
	int save_version = json_integer_value(json_object_get(save_data, "save_version"));
	if (save_version < 200) {
		#ifdef DEBUG
		LOG_I("Classic save detected!");
		#endif
		load_save_json_upgrades(save_data, config, true);
	}
	else if (save_version != config->save_version) {
		// TODO in future: upgrade save from older version if needed
		LOG_E("Error: save version (%d) does not match internal version (%d)", save_version, config->save_version);
		json_decref(save_data);
		exit(ERR_SAVE_VERSION);
	}
	else {
		load_save_json_upgrades(save_data, config, false);
	}
	load_save_json_buttons(save_data, config);
	load_save_json_state(save_data, config);
	
	long last_saved = json_integer_value(json_object_get(save_data, "last_saved"));
	if (save_version >= 200) {
		unsigned int magic_missile = json_integer_value(json_object_get(save_data, "magic_missile"));
		if (magic_missile != calc_magic_missile(last_saved, config)) {
			LOG_E("Error: magic missile!");
			json_decref(save_data);
			exit(ERR_MAGIC_MISSILE);
		}
	}
	json_decref(save_data);
}

void check_for_saves(Config* config) {
	for (int i = 0; i < 4; ++i) {
		load_save_properties(config, i);
	}
}

unsigned int calc_magic_missile(const long timestamp, const Config* const config) {
	long r = 0;
	for (int i = 0; i < UPGRADES_END_OFFSET; ++i) {
		r += config->upgrades[i].count;
	}
	r ^= ((long)config->state->magic_count + (long)config->state->magic_per_second + timestamp);
	return (unsigned int)(r % 69420);
}

Config* load_config_from_file() {
	char* config_path = get_config_file_path();
	json_error_t error;
	json_t* j_config = load_json_file(config_path, &error);
	if (!j_config) {
		LOG_E("Error creating j_config in load_config_from_file\n");
		return NULL;
	}
	const unsigned int autosave_interval = json_integer_value(json_object_get(j_config, "autosave_interval"));
	const double fps = json_real_value(json_object_get(j_config, "fps"));
	const E_AspectType aspect = json_integer_value(json_object_get(j_config, "aspect"));
	Config* config = init_magics_config(aspect, fps, autosave_interval);
	if (!config) {
		LOG_E("Error creating config in load_config_from_file\n");
	}
	return config;
}

int create_config_file(const unsigned int autosave_interval, const double fps, const E_AspectType aspect) {
	json_t* j_config = json_object();
	if (!j_config) {
		LOG_E("Error creating j_config object for config file\n");
		return -1;
	}
	json_object_set(j_config, "autosave_interval", json_integer(autosave_interval));
	json_object_set(j_config, "aspect", json_integer(aspect));
	json_object_set(j_config, "fps", json_real(fps));

	char* config_path = get_config_file_path();
	if (write_json_file(j_config, config_path) < 0) {
		LOG_E("Error writing config file: %s\n", config_path);
		json_decref(j_config);
		return -1;
	} else {
		#ifdef DEBUG
		LOG_D("Wrote config file to: %s\n", config_path);
		#endif
	}
	
	json_decref(j_config);
	return 0;
}

int create_default_config_file() {
	return create_config_file(DEFAULT_AUTOSAVE, DEFAULT_FPS, DEFAULT_ASPECT);
}

bool config_file_exists() {
	return file_exists(get_config_file_path());
}
