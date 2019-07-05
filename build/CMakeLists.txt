cmake_minimum_required (VERSION 3.0)
# Name our project
project (fm_smoov)
# Create a variable that holds the path to our libwiringPi.so file
#set (WPI_PATH /home/alex/Development/wiringPi/wiringPi)
set (ALSA_LIB_PATH /opt/repos/rpi_cross/external_libs/alsa-lib-built/lib)
set (ALSA_INCLUDE_PATH /opt/repos/rpi_cross/external_libs/alsa-lib-built/include)
set (FMSMOOV_INCLUDE_PATH /opt/repos/rpi_cross/fmsmoov/include)
# Add the local ‘include’ directory and the wiringPi directory to grab headers
#include_directories (include ${WPI_PATH})
include_directories (include ${ALSA_INCLUDE_PATH} ${FMSMOOV_INCLUDE_PATH})
# Actually find the wiringPi library object
#find_library(WPI_LIB wiringPi HINTS ${WPI_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(ALSA_LIB libasound.so HINTS ${ALSA_LIB_PATH} NO_CMAKE_FIND_ROOT_PATH)
# Alert the user if we do not find it
#if(NOT WPI_LIB)
# message(FATAL_ERROR “wiringPi library not found”)
#endif()
# Add all the *.c files in our source directory to our executable output
FILE(GLOB SRC_FILES ../src/*.cc)
add_executable(fm_smoov ${SRC_FILES})
target_link_libraries(fm_smoov pthread stdc++ m ${ALSA_LIB})
# Link the pre-compiled wiringPi library to the executable we just declared 
#target_link_libraries(blink_example ${WPI_LIB})
