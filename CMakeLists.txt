cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif(UNIX)

if(CMAKE_BUILD_TYPE EQUAL "Debug")
	set(CMAKE_CXX_FLAGS "$CMAKE_CXX_FLAGS} -g3")
endif()

project(snake_maker)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set(GLAD_GL "" )
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glfw)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/glm)

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
  src/settings.cpp
  src/increment_value.cpp
  src/tile_create.cpp
  src/slider.cpp
  src/text_box.cpp
  src/map_create.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)


target_include_directories(${PROJECT_NAME}
	PUBLIC
       ${CMAKE_SOURCE_DIR}/external/include/
       ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(${PROJECT_NAME}
	glfw
	glm
)

