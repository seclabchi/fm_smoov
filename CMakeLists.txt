
cmake_minimum_required(VERSION 2.8.11)

# Project name
project(fmsmoovd)

# This setting is useful for providing JSON file used by CodeLite for code completion
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Set default locations
set(CL_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}//cmake-build-Debug/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})

set(CONFIGURATION_NAME "Debug")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -ggdb -O0")

# Projects
include_directories(hdr)
include_directories(/usr/include/alsa)

set(SOURCES src/main.cpp src/device_db.cpp src/audio_hub.cpp src/command_handler.cpp
    src/pcm_device.cpp src/tone_generator.cpp src/processor_analyzer.cpp src/processor_simple_gain.cpp
    src/pcm_playback.cpp src/pcm_capture.cpp src/stream_capture.cpp src/processor.cpp src/logger.cpp
    src/soundfile.cpp)
    
add_executable(fmsmoovd ${SOURCES})
target_link_libraries(fmsmoovd asound pthread sndfile)

# Top project
