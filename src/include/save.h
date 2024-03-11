#ifndef MAGICS_SAVE_H
#define MAGICS_SAVE_H

#include <jansson.h>
#include "object.h"

extern const char* UPGRADE_DATA[];

void print_json(json_t *root);
void print_json_aux(json_t *element, int indent);
void print_json_indent(int indent);
const char *json_plural(size_t count);
void print_json_object(json_t *element, int indent);
void print_json_array(json_t *element, int indent);
void print_json_string(json_t *element, int indent);
void print_json_integer(json_t *element, int indent);
void print_json_real(json_t *element, int indent);
void print_json_true(json_t *element, int indent);
void print_json_false(json_t *element, int indent);
void print_json_null(json_t *element, int indent);

json_t *load_json(const char *text);
char *read_line(char *line, int max_chars);

json_t* create_save_json_upgrades(Config* config);
json_t* create_save_json_state(Config* config);
json_t* create_save_json_buttons(Config* config);
json_t* create_save_json(Config* config);

int magics_dump_json(json_t* data, const char* path);

int save_game(Config* config, const unsigned short slot);

int load_save_json_upgrades(json_t* save_data, Config* config, const bool is_classic);
void load_save_json_state(json_t* save_data, Config* config);
void load_save_json_buttons(json_t* save_data, Config* config);
json_t* magics_load_json(const char* path);
json_t* load_save_json(const char* path);

char* create_save_path();

void load_save_properties(Config* config, const unsigned short slot);
void load_save(Config* config, const unsigned short slot);

void check_for_saves(Config* config);

unsigned int calc_magic_missile(const long timestamp, const Config* const config);

#endif