
if (MSVC)
    add_compile_options(/utf-8)
endif ()

add_executable(pcm_record pcm_record.cpp)
add_executable(resample_pcm resample_pcm.cpp)

target_link_libraries(pcm_record PRIVATE ffmpeg)
target_link_libraries(resample_pcm PRIVATE ffmpeg)
