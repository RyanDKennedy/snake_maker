cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(snake_maker)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set( GLAD_GL "" )
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glfw)

add_executable(${PROJECT_NAME}
  src/main.cpp
  external/glad.c
  src/quad.cpp
  src/shader.cpp
  src/utils.cpp
  src/stbi_implementation.cpp
  src/custom_fonts.cpp
  src/drawing.cpp
  src/pixel_map.cpp
  src/collisions.cpp
  src/button.cpp
  src/menu.cpp
  src/snake.cpp
  src/map.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)


target_include_directories(${PROJECT_NAME}
	PUBLIC
       ${CMAKE_SOURCE_DIR}/external/include/
       ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(${PROJECT_NAME} glfw)

