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
set_target_properties(mstd-download PROPERTIES OUTPUT_NAME mstd)

ExternalProject_Add(
	glfw-download
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
)
set_target_properties(glfw-download PROPERTIES OUTPUT_NAME glfw)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src" "*.cpp" "*.c")

add_library(tiling ${SOURCES})
target_include_directories(tiling PUBLIC inc/ ${INSTALL_LOCATION}/include)
target_link_directories(tiling PUBLIC ${INSTALL_LOCATION}/include)
target_link_libraries(tiling PRIVATE glfw GLU)
set_target_properties(tiling PROPERTIES CXX_STANDARD 20)

set(TILING_BUILD_TESTS TRUE CACHE BOOL "Build tiling test programs")
if (${TILING_BUILD_TESTS})
	add_subdirectory("tests/panels")
endif()
