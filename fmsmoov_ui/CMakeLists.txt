cmake_minimum_required(VERSION 2.8.11)

# Project name
project(fmsmoov_ui)

# This setting is useful for providing JSON file used by CodeLite for code completion
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Set default locations
set(CL_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../cmake-build-Debug/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})

set(CONFIGURATION_NAME "Debug")

# Projects
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -ggdb -O0")

# Projects
include_directories(hdr)
#include_directories(/usr/include/alsa)

set(SOURCES 
    src/main.cpp )
    
add_executable(fmsmoov_ui ${SOURCES})
target_link_libraries(fmsmoov_ui fltk Xext X11 pthread m)

# Top project
