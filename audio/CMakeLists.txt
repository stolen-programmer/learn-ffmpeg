
if (MSVC)
    add_compile_options(/utf-8)
endif ()

add_executable(pcm_record pcm_record.cpp)

target_link_libraries(pcm_record PRIVATE ffmpeg)
