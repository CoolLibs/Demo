cmake_minimum_required(VERSION 3.16)

# Set project name
project(CoolDemo C CXX)

# Set C++ version
set(CXX_STANDARD 17)

# Choose our GPU API
#set(COOL_USE_VULKAN true)
set(COOL_USE_OPENGL true)

# Set the folder where the executable is created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Grab all the source files
file(GLOB_RECURSE MY_SOURCES
    src/*
    Cool/src/*
)

# Create target executable
add_executable(${PROJECT_NAME}
    ${MY_SOURCES}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CXX_STANDARD})

# #define CMAKE_SOURCE_DIR
# This is useful to set the current working directory in debug mode. YOUR RELEASE CODE SHOULDN'T RELY ON CMAKE_SOURCE_DIR
set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_DEFINITIONS "CMAKE_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

# Include Cool
include("Cool/cmake/IncludeMe.cmake")

# Add a post build operation to copy shaders to the output folder (where the executable is created)
include("Cool/cmake/copy_folder_to_the_output_folder.cmake")
copy_folder_to_the_output_folder("shaders")

# Set include directories
include_directories(
    src
)