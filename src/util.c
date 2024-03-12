#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "include/enum.h"
#include "include/util.h"

// Platform-dependent
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define F_OK 0
#define access _access
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
		LOG_E("Cannot calloc memory at %p.", ptr);
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
		LOG_E("Cannot malloc memory at %p.", ptr);
		exit(ERR_MALLOC);
	}
	return ptr;	
}

const char* human_format_to_float(long double num, const int precision) {
	static const char* suffix[] = {"", "K", "M", "G", "T", "P", "E", "Z", "Y", "KY", "MY", "GY", "TY", "PY", "EY", "ZY", "YY", " WHAT THE...?"};
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
			LOG_E("Error creating directory: %s", path);
            return -1; // Failure
        }
		LOG_W("Directory exists: %s", path);
		return 0;
    }
    #else
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0777) == -1) {
            LOG_E("Error creating directory: %s", path);
            return -1;
        }
    }
    #endif

    return 0;
}

bool file_exists(const char* path) {
	return (access(path, F_OK) == 0);
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
	config->screen_width = m.w / ratio;
	config->screen_height = m.h / ratio;
	config->screen_scale = (double)ratio;

	return 0;
}
#endif

// ANDROID-RELATED FUNCTIONS
#ifdef __ANDROID__

AAssetManager* ASSET_MGR = NULL;

/*
Obtains the AAssetManager from Java. Needed for accessing files in the app assets.
*/
void android_set_asset_mgr() {
	JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
	jobject activity = (jobject)SDL_AndroidGetActivity();
	jclass clazz = (*env)->GetObjectClass(env, activity);

	jmethodID methodID = (*env)->GetMethodID(env, clazz, "getAssets", "()Landroid/content/res/AssetManager;");
	jobject assetManager = (*env)->CallObjectMethod(env, activity, methodID);
	ASSET_MGR = AAssetManager_fromJava(env, assetManager);

	(*env)->DeleteLocalRef(env, activity);
	(*env)->DeleteLocalRef(env, clazz);
}

/*
Frees the AAssetManager pointer.
*/
void android_free_asset_mgr() {
	if (ASSET_MGR) free(ASSET_MGR);
}

/*
Loads "filename" from the Android app assets, then dumps it to the internal storage.
*/
int android_load_asset_file(const char* filename) {
	if (!ASSET_MGR) {
		LOG_D("Getting asset manager...\n");
		android_set_asset_mgr();
	}
	
	AAsset* file = AAssetManager_open(ASSET_MGR, filename, AASSET_MODE_BUFFER);
	size_t file_len = AAsset_getLength(file);

	LOG_D("Asset file \"%s\" len: %d", filename, file_len);

	// Allocate memory to read file
	char* content = (char*)safe_malloc(file_len+1);
	content[file_len] = '\0';

	// Read file
	AAsset_read(file, content, file_len);

	SDL_RWops* fp = SDL_RWFromFile(filename, "w");
	if (!fp) {
		LOG_E("Error opening output fp in load_asset_file\n");
		return -1;
	}
	SDL_RWwrite(fp, content, file_len, 1);
	SDL_RWclose(fp);

	// Free the memory allocated earlier
	free(content);
	return 0;
}
#endif