cmake_minimum_required(VERSION 3.5)
project(terrain)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

include_directories(include)

set(SOURCE_FILES
  src/glad.c

  src/window.cpp
  src/shader.cpp
  src/config.cpp
  src/exporter.cpp
  src/terrain.cpp
  src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} glfw)
