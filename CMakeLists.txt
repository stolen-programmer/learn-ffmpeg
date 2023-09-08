cmake_minimum_required(VERSION 3.26)
project(learn_ffmpeg)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options(/utf-8)
endif ()

#find_package(FFMPEG REQUIRED)

add_library(ffmpeg INTERFACE)

target_include_directories(ffmpeg INTERFACE ${CMAKE_SOURCE_DIR}/third-party/ffmpeg/include)

target_link_directories(ffmpeg INTERFACE ${CMAKE_SOURCE_DIR}/third-party/ffmpeg/lib)

target_link_libraries(ffmpeg INTERFACE avcodec avdevice avformat avutil swresample swscale)

#target_link_libraries(record PRIVATE ffmpeg)

#target_link_libraries(resample_audio PRIVATE ffmpeg)

#target_link_libraries(mux PRIVATE ffmpeg)

add_subdirectory(audio)