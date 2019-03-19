cmake_minimum_required(VERSION 3.14)

add_subdirectory(external/FTXUI)

find_package(ALSA REQUIRED)

include_directories(${ALSA_INCLUDE_DIRS})

add_executable(volume src/volume.cpp)
target_link_libraries(volume PUBLIC component ${ALSA_LIBRARY})
