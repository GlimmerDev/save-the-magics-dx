cmake_minimum_required(VERSION 3.1)

project(
  FontCache
  VERSION 0.10.0
  LANGUAGES C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT CMAKE_BUILD_TYPE)
  # Run cmake -DCMAKE_BUILD_TYPE=Release for a release build
  set(CMAKE_BUILD_TYPE Debug)
endif()

option(FontCache_BUILD_TESTS "Build tests" ON)

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)

add_library(FontCache STATIC ${FontCache_SOURCE_DIR}/SDL_FontCache.h
                             ${FontCache_SOURCE_DIR}/SDL_FontCache.c)

set_target_properties(
  FontCache
  PROPERTIES C_STANDARD 11
             C_STANDARD_REQUIRED ON
             C_EXTENSIONS OFF
             OUTPUT_NAME fontcache
             FOLDER "FontCache")

target_link_libraries(FontCache PUBLIC ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})

target_include_directories(
  FontCache
  PUBLIC $<BUILD_INTERFACE:${FontCache_SOURCE_DIR}> $<INSTALL_INTERFACE:include>
  PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

set(FontCache_INCLUDE_DIR ${FontCache_SOURCE_DIR})

if(FontCache_BUILD_TESTS)
  add_subdirectory(test)
endif()
