cmake_minimum_required(VERSION 3.15)
project(raytris)

find_package(raylib 3.0 REQUIRED) # Requires at least version 3.0
find_package(PhysFS 3.0 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For clangd to be happy
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES src/*.c src/*.cpp)

link_directories(/opt/homebrew/lib)

add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(/opt/homebrew/include)
include_directories(${PROJECT_NAME} src/*)

target_include_directories(${PROJECT_NAME} PRIVATE
  /opt/homebrew/include
  src
)

target_link_libraries(${PROJECT_NAME}
  raylib
  physfs
)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
  target_link_libraries(${PROJECT_NAME}
    "-framework IOKit"
    "-framework Cocoa"
    "-framework OpenGL"
  )
endif()
