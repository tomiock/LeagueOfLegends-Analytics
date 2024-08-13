cmake_minimum_required(VERSION 3.10)

# Set project name
project(opencv_lol VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Set source files
set(SOURCES
    cpp/main.cpp
    cpp/detect_champ.cpp
    cpp/remove_terrain.cpp
    # cpp/circle_priority.cpp
)

# compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Installation target
install(TARGETS ${PROJECT_NAME} DESTINATION ../.)

