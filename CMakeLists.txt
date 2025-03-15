cmake_minimum_required(VERSION 3.12)

project(
	"tiling" VERSION 1.0.0
	DESCRIPTION "OpenGL tile renderer"
	HOMEPAGE_URL "https://github.com/mopo3ob-jack/tiling"
)

include(GNUInstallDirs)
include(FetchContent)

FetchContent_Declare(
	mstd
	GIT_REPOSITORY https://github.com/mopo3ob-jack/mstd
)
FetchContent_MakeAvailable(SFML mstd)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tiling src/main.cpp)
target_include_directories(tiling PRIVATE ${CMAKE_INCLUDE_DIRECTORY})
target_sources(tiling PRIVATE src/TileRenderer.cpp)

target_link_libraries(tiling PRIVATE glfw)
target_link_libraries(tiling PRIVATE mstd)
