cmake_minimum_required(VERSION 3.12)

project(panels VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src" "*.cpp" "*.c")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC inc/)
target_link_libraries(${PROJECT_NAME} PRIVATE tiling)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
