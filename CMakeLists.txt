cmake_minimum_required(VERSION 3.26)
project(learn_ffmpeg)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options(/utf-8)
endif ()

find_package(FFMPEG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_library(ffmpeg INTERFACE)

target_include_directories(ffmpeg INTERFACE ${FFMPEG_INCLUDE_DIRS})

target_link_directories(ffmpeg INTERFACE ${FFMPEG_LIBRARY_DIRS})

target_link_libraries(ffmpeg INTERFACE ${FFMPEG_LIBRARIES})

add_subdirectory(audio)