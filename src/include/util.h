#ifndef MAGICS_UTIL_H
#define MAGICS_UTIL_H

#include <stdbool.h>
#include <jansson.h>
#include "object.h"

// MOBILE (SHARED)
#ifdef __MAGICSMOBILE__
int mobile_set_screen_dims(Config* config);
#endif

// ANDROID
#ifdef __ANDROID__
#include <android/log.h>

#define  LOG_TAG    "savethemagics-dx"
#define  LOG_E(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOG_W(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOG_D(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOG_I(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int android_load_asset_file(const char* filename);

// IPHONE
//#elif __IPHONE__

#else

#define  LOG_E(...)  printf("ERROR: "); fprintf(stderr, __VA_ARGS__)
#define  LOG_W(...)  printf("WARN: "); fprintf(stderr, __VA_ARGS__)
#define  LOG_D(...)  printf("DEBUG: "); printf(__VA_ARGS__)
#define  LOG_I(...)  printf("INFO: "); printf(__VA_ARGS__)

#endif

const char* frames_to_time_str(const unsigned int frames, const GameState* state);

const char* time_to_time_str(const time_t* const timestamp);

void* safe_calloc(size_t nmemb, size_t size);

void* safe_malloc(size_t size);

const char* human_format_to_float(long double num, const int precision);

const char* human_format_to_int(long double num);

const char* human_format(long double num);

bool string_startswith(const char* input, const char* match);

int create_directory(const char* path);

bool file_exists(const char* path);

int _scr_center(const bool y, Config* const cptr, bool set_cptr);

int _scr_dims(const bool h, Config* const cptr, bool set_cptr);

int screen_center_set_cptr(Config* const cptr);

int screen_dims_set_cptr(Config* const cptr);

double screen_ratio();

int screen_width();

int screen_height();

int screen_center_x();

int screen_center_y();

double _get_fps(Config* const cptr, bool set_cptr);

double get_fps_set_cptr(Config* const cptr);

double get_fps();

char* magics_read_file(const char* path, size_t* size);
int magics_write_file(const char* data, const char* path, size_t size);

json_t* load_json_file(const char* path, json_error_t* error);
int write_json_file(json_t* data, const char* path);

#endif
