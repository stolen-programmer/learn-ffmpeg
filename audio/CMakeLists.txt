
if (MSVC)
    add_compile_options(/utf-8)
endif ()

add_executable(pcm_record pcm_record.cpp)
add_executable(resample_pcm resample_pcm.cpp)
add_executable(audio_fifo audio_fifo.cpp)
add_executable(encode_aac encode_aac.cpp)

target_link_libraries(pcm_record PRIVATE ffmpeg)
target_link_libraries(resample_pcm PRIVATE ffmpeg)
target_link_libraries(audio_fifo PRIVATE ffmpeg)
target_link_libraries(encode_aac PRIVATE ffmpeg)
