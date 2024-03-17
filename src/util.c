#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "include/enum.h"
#include "include/util.h"

// Platform-dependent includes
// Windows
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define F_OK 0
#define access _access_s
// POSIX
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#endif

/*
Converts an integer frame count into a formatted time string.
*/
const char* frames_to_time_str(const unsigned int frames, const GameState* state) {
    static char buffer[10];
    unsigned int total_seconds = frames / get_fps();
    unsigned int minutes = total_seconds / 60;
    unsigned int seconds = total_seconds % 60;
    sprintf(buffer, "%d:%02d", minutes, seconds);
    return buffer;
}

/*
Converts an epoch timestamp into a formatted time string.
*/
const char* time_to_time_str(const time_t* const timestamp) {
	static char buf[128];
	struct tm* tm_struct = localtime(timestamp);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_struct);
	return buf;
}

/*
"Safe" version of calloc that will exit the program if it fails.
This should NOT be used until the config pointers are registered, otherwise some pointers may not be freed on exit.
*/
void* safe_calloc(size_t nmemb, size_t size) {
	void* ptr = calloc(nmemb, size);
	if (!ptr) {
		LOG_E("Cannot calloc memory at: %p\n", ptr);
		exit(ERR_MALLOC);
	}
	return ptr;
}

/*
"Safe" version of malloc that will exit the program if it fails.
This should NOT be used until the config pointers are registered, otherwise some pointers may not be freed on exit.
*/
void* safe_malloc(size_t size) {
	void* ptr = malloc(size);
	if (!ptr) {
		LOG_E("Cannot malloc memory at: %p\n", ptr);
		exit(ERR_MALLOC);
	}
	return ptr;	
}

const char* human_format_to_float(long double num, const int precision) {
	static const char* suffix[] = {"", "K", "M", "G", "T", "P", "E", "Z", "Y", "KY", \
									"MY", "GY", "TY", "PY", "EY", "ZY", "YY", " WHAT THE...?"};
    static char res_buf[20];
	
	if (num == 0) {
		sprintf(res_buf, "%.2Lf", num);
		return res_buf;
	}
   
    const int magnitude = (int)log10l(num) / 3;
    num /= powl(1000.0, (long double)magnitude);
    sprintf(res_buf, "%.*Lf%s", precision, num, suffix[magnitude > 16 ? 17 : magnitude]);

    return res_buf;
}
const char* human_format_to_int(long double num) {
	return human_format_to_float(num, 0);
}

const char* human_format(long double num) {
 	return human_format_to_float(num, 2);
}

bool string_startswith(const char* input, const char* match) {
    if ((!input)||(!match)) return false;
	// match length zero, always true
    const int match_len = strlen(match);
    if (match_len == 0) return true;
	// match longer than input, always false
    const int in_len = strlen(input);
    if (in_len < match_len) return false;
	// same length, save time with strcmp
    if (in_len == match_len) return (strcmp(input, match) == 0);
	// else, do it the slow way
    for (int i = 0; i < match_len; ++i) {
        if (input[i] != match[i]) return false;
    }
    return true;
}

int create_directory(const char* path) {
    #ifdef _WIN32
    if (!CreateDirectory(path, NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
			LOG_E("Error creating directory: %s\n", path);
            return -1; // Failure
        }
		LOG_W("Directory exists: %s\n", path);
		return 0;
    }
    #else
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0777) == -1) {
            LOG_E("Error creating directory: %s\n", path);
            return -1;
        }
    }
    #endif

    return 0;
}

/*
 * Checks if a file exists and returns the result.
*/
bool file_exists(const char* path) {
	bool res;
    if (access(path, 0) == F_OK)
        res = true;
    else
        res = false;
	// LOG_D("File %s exists? %d", path, (int)res);
	return res;
}

int _scr_center(const bool y, Config* const cptr, bool set_cptr) {
	static Config* config = NULL;
	if (set_cptr) {
		config = cptr;
		return 0;
	} 
	else if (!config) {
		return -1;
	}
	else if (y) {
		return config->screen_center_y;
	} 
	return config->screen_center_x;
}

int _scr_dims(const bool h, Config* const cptr, bool set_cptr) {
	static Config* config = NULL;
	if (set_cptr) {
		config = cptr;
		return 0;
	} 
	else if (!config) {
		return -1;
	}
	else if (h) {
		return config->screen_height;
	}
	return config->screen_width;
}

int screen_center_set_cptr(Config* const cptr) {
	return _scr_center(false, cptr, true);
}

int screen_dims_set_cptr(Config* const cptr) {
	return _scr_dims(false, cptr, true);
}

double screen_ratio() {
	return ((double)_scr_dims(false, NULL, false)) /
			((double)_scr_dims(true, NULL, false));
}

int screen_width() {
	return _scr_dims(false, NULL, false);
}

int screen_height() {
	return _scr_dims(true, NULL, false);
}

int screen_center_x() {
	return _scr_center(false, NULL, false);
}

int screen_center_y() {
	return _scr_center(true, NULL, false);
}

double _get_fps(Config* const cptr, bool set_cptr) {
	static Config* config = NULL;
	if (set_cptr) {
		config = cptr;
		return 0.0;
	} 
	else if (!config) {
		return NAN;
	}
	return config->FPS;
}

double get_fps_set_cptr(Config* const cptr) {
	return _get_fps(cptr, true);
}

double get_fps() {
	return _get_fps(NULL, false);
}

char* magics_read_file(const char* path, size_t* size) {
	SDL_RWops* fp = SDL_RWFromFile(path, "rb");
	if (!fp) {
		LOG_E("Error opening file for load: %s\n", path);
		return NULL;
	}
	int len = SDL_RWseek(fp, 0, SEEK_END);
	(*size) = len+1;
	SDL_RWseek(fp, 0, SEEK_SET);
	char* data_str = (char*)safe_malloc(len+1);
	data_str[len] = '\0';
	SDL_RWread(fp, data_str, len, 1);
	SDL_RWclose(fp);
	
	return data_str;
}

int magics_write_file(const char* data, const char* path, size_t size) {
	LOG_D("Write file path: %s\n", path);
	SDL_RWops* fp = SDL_RWFromFile(path, "wb");
	if (!fp) {
		LOG_E("Error opening file for write: %s\n", path);
		return -1;
	}
	SDL_RWwrite(fp, data, size, 1);
	SDL_RWclose(fp);
	
	return 0;
}

/*
Loads a JSON file into a json_t object.
Uses SDL file I/O functions instead of jansson to ensure cross-platform compatibility.
*/
json_t* load_json_file(const char* path, json_error_t* error) {
	size_t size;
	char* data_str = magics_read_file(path, &size);
	if (!data_str) {
		return (json_t*)NULL;
	}
	json_t* d = json_loads(data_str, 0, error);
	free(data_str);
	return d;
}

/*
Dumps a json_t object into a file.
Uses SDL file I/O functions instead of jansson to ensure cross-platform compatibility.
*/
int write_json_file(json_t* data, const char* path) {
	char* data_str = json_dumps(data, JSON_ENSURE_ASCII|JSON_INDENT(4));
	if (!data_str) return -1;
	size_t size = strlen(data_str);
	if (magics_write_file(data_str, path, size) < 0) {
		return -1;
	}
	free(data_str);
	return 0;
}

// ***************************
// * SHARED MOBILE FUNCTIONS *
// *************************** 
#ifdef __MAGICSMOBILE__
/*
Determines the dimensions and scale ratio when running on mobile.
*/
int mobile_set_screen_dims(Config* config){
	SDL_DisplayMode m;
	int res = SDL_GetCurrentDisplayMode(0, &m);
	if (res < 0) {
		LOG_E("Error getting display mode");
		return -1;
	}
	unsigned int ratio = floor(m.h/600);
	LOG_D("ScreenInfo: w: %d , h: %d, r: %d", m.w, m.h, ratio);
	config->screen_width = m.w/ratio;
	config->screen_height = m.h/ratio;
	config->screen_scale = (double)ratio;
	return 0;
}
#endif

// ANDROID-RELATED FUNCTIONS
#ifdef __ANDROID__
int android_load_asset_file(const char* filename) {
	size_t size;
	char* data = magics_read_file(filename, &size);
	if (!data) {
		LOG_E("Error reading file from assets: %s", filename);
		return -1;
	}
	LOG_D("%s - size %d", filename, size);
	if (magics_write_file(data, filename, size) < 0) {
		LOG_E("Error writing file loaded from asset: %s", filename);
		return -1;
	}
	free(data);
	return 0;
}
#endif
