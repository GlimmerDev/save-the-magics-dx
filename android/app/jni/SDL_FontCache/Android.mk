LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Path to SDL2 and SDL2_ttf libraries
SDL2_PATH := ../SDL
SDL2_TTF_PATH := ../SDL2_ttf

# SDL_FontCache module
LOCAL_MODULE := FontCache
LOCAL_SRC_FILES := SDL_FontCache.c

# Include directories for SDL2 and SDL2_ttf
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL2_PATH)/include \
                    $(LOCAL_PATH)/$(SDL2_TTF_PATH)/include

# Link against SDL2 and SDL2_ttf libraries
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf

include $(BUILD_SHARED_LIBRARY)
