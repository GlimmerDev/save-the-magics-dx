LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_SRC_PATH = ../../../../src

SDL_PATH := ../SDL
SDL_TTF_PATH := ../SDL2_ttf
SDL_MIXER_PATH := ../SD2_mixer
SDL_FC_PATH := ../SDL_FontCache
JANSSON_PATH := ../jansson

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/$(SDL_TTF_PATH)/include \
                    $(LOCAL_PATH)/$(SDL_MIXER_PATH)/include \
                    $(LOCAL_PATH)/$(SDL_FC_PATH) \
                    $(LOCAL_PATH)/$(JANSSON_PATH)/src \
                    $(LOCAL_PATH)/$(JANSSON_PATH)/android

# Add C source files from the top-level src directory
LOCAL_SRC_FILES := 	$(LOCAL_SRC_PATH)/main.c \
			$(LOCAL_SRC_PATH)/draw.c \
			$(LOCAL_SRC_PATH)/event.c \
			$(LOCAL_SRC_PATH)/object.c \
			$(LOCAL_SRC_PATH)/save.c \
			$(LOCAL_SRC_PATH)/util.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer SDL2_ttf FontCache libjansson

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
