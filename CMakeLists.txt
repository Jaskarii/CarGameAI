cmake_minimum_required(VERSION 3.0)

project(MyOpenGL)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Libraries/include/GLFW)
set(GLFW_LIB_DIR ${CMAKE_SOURCE_DIR}/Libraries/lib)

# Include GLFW headers
include_directories(${GLFW_INCLUDE_DIR})

# Find and link GLFW library
find_library(GLFW_LIBRARY glfw3 PATHS ${GLFW_LIB_DIR})


# Include directories for header files
include_directories(
    include 
    headers
    Libraries/include
)

# Add glad.c to your project
add_library(glad glad.c)

file(GLOB IMGUISOURCES "Libraries/ImGui/*.cpp")

add_library(imgui ${IMGUISOURCES})

# Source files
file(GLOB SOURCES "src/*.cpp")

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# Copy shader files to the build directory
file(GLOB SHADER_FILES "${SHADER_DIR}/*.glsl")
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# Create executable
add_executable(MyOpenGLApp ${SOURCES})

target_link_libraries(MyOpenGLApp
    imgui
    glad  # Example for linking against OpenGL
    ${GLFW_LIBRARY}
)

