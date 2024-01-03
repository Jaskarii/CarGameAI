cmake_minimum_required(VERSION 3.0)

project(MyOpenGL)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)


find_package(glfw3 REQUIRED)

# Include directories for header files
include_directories(
    include 
    headers
    Libraries/include
)

# Add glad.c to your project
add_library(glad glad.c)

# Source files
file(GLOB SOURCES "src/*.cpp")

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# Copy shader files to the build directory
file(GLOB SHADER_FILES "${SHADER_DIR}/*.shader")
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# Create executable
add_executable(MyOpenGLApp ${SOURCES})

target_link_libraries(MyOpenGLApp
    glad  # Example for linking against OpenGL
    glfw dl
)

