#OBJS specifies which files to compile as part of the project
OBJS = src/main.c src/util.c src/draw.c src/object.c src/event.c src/save.c

#CC specifies which compiler we're using
CC = gcc

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = magics

# Windows
ifeq ($(OS),Windows_NT)
	INCLUDE_PATHS = -IC:\mingw_lib\include -IC:\mingw_lib\include\SDL2
	LIBRARY_PATHS = -LC:\mingw_lib\lib

	# -w suppresses all warnings
	# -Wl,-subsystem,windows gets rid of the console window
	COMPILER_FLAGS = -D__USE_MINGW_ANSI_STDIO

	LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_ttf -lSDL_FontCache -ljansson 
# Unix-like (MacOS, Linux)
else
	INCLUDE_PATHS = -I/usr/local/include -I/usr/local/include/SDL2
	COMPILER_FLAGS = 
	LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_ttf -lFontCache -ljansson -lm
endif

# Only builds PC version
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# Only builds Android
android:
	cd android && ./gradlew assembleDebug
	cp android/app/build/outputs/apk/debug/app-debug.apk /mnt/vm-shared/
	cd ..

debug: COMPILER_FLAGS += -ggdb -pg -O0 -DDEBUG
debug: all

debug-android: COMPILER_FLAGS += -DDEBUG
debug-android: android

.PHONY: all android debug debug-android
