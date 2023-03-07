cmake_minimum_required(VERSION 3.5)
project (
  capture-cam
  LANGUAGES CXX
)

get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)
if ("${srcdir}" STREQUAL "${bindir}")
  message(FATAL_ERROR "Please use 'build' dir; Try '-Bbuild' option")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -Werror -g")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
  "main.cpp"
)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

