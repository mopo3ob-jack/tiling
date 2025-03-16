cmake_minimum_required(VERSION 3.12)

project(
	tiling VERSION 1.0.0
	DESCRIPTION "OpenGL tile renderer"
	HOMEPAGE_URL "https://github.com/mopo3ob-jack/tiling"
)

include(GNUInstallDirs)
include(FetchContent)

FetchContent_Declare(
	mstd
	GIT_REPOSITORY https://github.com/mopo3ob-jack/mstd
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src" "*.cpp" "*.c")

add_library(tiling ${SOURCES})
target_include_directories(tiling PUBLIC inc/)
target_link_libraries(tiling PRIVATE glfw GLU)
set_target_properties(tiling PROPERTIES CXX_STANDARD 20)

set(BUILD_TESTS TRUE CACHE BOOL "Build library tests")
if (${BUILD_TESTS})
	add_subdirectory("tests/panels")
endif()
