cmake_minimum_required(VERSION 3.20)

project(CoolDemo)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}) # Set the folder where the executable is created

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion -Wimplicit-fallthrough)
endif()

# Maybe enable warnings as errors
set(WARNINGS_AS_ERRORS_FOR_COOLDEMO OFF CACHE BOOL "ON iff you want to treat warnings as errors") # Might be overriden in the CMake cache

if(WARNINGS_AS_ERRORS_FOR_COOLDEMO)
    if(MSVC)
        target_compile_options(Coollab-Properties INTERFACE /WX)
    else()
        target_compile_options(Coollab-Properties INTERFACE -Werror)
    endif()
endif()

# TODO(WebGPU) Remove
# Choose our GPU API and its version
# set(COOL_USE_VULKAN 110)
set(COOL_USE_OPENGL $<IF:$<PLATFORM_ID:Darwin>,410,430>) # Must be >= 330 for ImGui to work properly # We use 410 when building on Mac because it doesn't support higher versions of OpenGL

# Include Cool
add_subdirectory(Cool)
target_link_libraries(${PROJECT_NAME} PRIVATE Cool::Core)
cool_setup(${PROJECT_NAME})

# Set app icon
Cool__set_app_icon(${PROJECT_NAME} "res/logo.png" "${CMAKE_SOURCE_DIR}/app-resources/icon.rc")

# Grab all the source files
file(GLOB_RECURSE COOL_DEMO_SOURCES CONFIGURE_DEPENDS src/*)
target_sources(${PROJECT_NAME} PRIVATE ${COOL_DEMO_SOURCES})

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Copy resources to the output folder (where the executable is created) after each build
include("Cool/CMakeUtils/files_and_folders.cmake")
Cool__target_copy_folder(${PROJECT_NAME} "shaders")
