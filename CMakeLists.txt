cmake_minimum_required(VERSION 3.20)
project(audio_rng VERSION 1.0)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add miniaudio.c
set(MINIAUDIO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/miniaudio.c)

add_executable(audio_rng 
    src/main.cpp 
    src/Audio.cpp 
    src/EntropyCollector.cpp
    ${MINIAUDIO_SRC}
)

target_include_directories(audio_rng PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Libraries
find_package(Threads REQUIRED)
target_link_libraries(audio_rng PRIVATE Threads::Threads m stdc++exp)

if(WIN32)
    target_link_libraries(audio_rng PRIVATE winmm ole32 user32 advapi32)
elseif(UNIX)
    target_link_libraries(audio_rng PRIVATE dl)
endif()
