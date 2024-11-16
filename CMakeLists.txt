cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(SOURCE_FILES
  src/main.cpp
  src/glad.c
  src/quad.cpp
  src/shader.cpp
  src/utils.cpp
  src/stbi_implementation.cpp
  src/custom_fonts.cpp
  src/drawing.cpp
  src/pixel_map.cpp
)

project(snake_maker)

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
	PUBLIC
       ${CMAKE_SOURCE_DIR}/include/
       ${CMAKE_SOURCE_DIR}/src/
)
target_link_libraries(${PROJECT_NAME} glfw)
