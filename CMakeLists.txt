cmake_minimum_required(VERSION 3.14)

project(i3-termdialogs)
include(FetchContent)

# ftxui ------------------------------------------------------------------------
FetchContent_Declare(ftxui 
	GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
	GIT_TAG  f506be941ddda760bdc45e1593362b383d77eabd
)
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ALSA -------------------------------------------------------------------------
find_package(ALSA REQUIRED)

# volume -----------------------------------------------------------------------
add_executable(volume src/volume.cpp)
target_link_libraries(volume PRIVATE ftxui::component)
target_link_libraries(volume PRIVATE ALSA::ALSA)

# C++17 is used. We requires fold expressions at least.
set_target_properties(volume PROPERTIES CXX_STANDARD 17)
