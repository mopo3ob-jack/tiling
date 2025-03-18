cmake_minimum_required(VERSION 3.12)

project(
	tiling VERSION 1.0.0
	DESCRIPTION "OpenGL tile renderer"
	HOMEPAGE_URL "https://github.com/mopo3ob-jack/tiling"
)

include(GNUInstallDirs)
include(ExternalProject)

set(INSTALL_LOCATION ${CMAKE_BINARY_DIR}/deps)

ExternalProject_Add(
	mstd-download
	GIT_REPOSITORY https://github.com/mopo3ob-jack/mstd.git
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
)

ExternalProject_Add(
	glfw-download
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")

add_library(
	tiling
	src/glad/glad.c
	src/tiling/stb_image.cpp
	src/tiling/TileShader.cpp
	src/tiling/TileRenderer.cpp
)
target_include_directories(tiling PUBLIC inc/ ${INSTALL_LOCATION}/include)
target_link_directories(tiling PUBLIC ${INSTALL_LOCATION}/include)
target_link_libraries(tiling PRIVATE glfw GLU)
set_target_properties(tiling PROPERTIES CXX_STANDARD 20)

set(TILING_BUILD_TESTS TRUE CACHE BOOL "Build tiling test programs")
if (${TILING_BUILD_TESTS})
	add_subdirectory("tests/panels")
endif()
