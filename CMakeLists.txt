cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(GEODE_DISABLE_PRECOMPILED_HEADERS ON)
set(USE_ZLIB OFF)

project(GeometryPlug VERSION 2.2.0)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    # Add your cpp files here
)

file(GLOB SRC_FILES    
    "src/*.cpp"
    "include/*.h"
)

target_sources(GeometryPlug PUBLIC
    ${SRC_FILES}
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})

find_package(Threads REQUIRED)
CPMAddPackage("gh:machinezone/IXWebSocket#e03c0be")
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.2)
target_link_libraries(${PROJECT_NAME} ixwebsocket::ixwebsocket nlohmann_json::nlohmann_json )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GeometryPlug PROPERTY CXX_STANDARD 11)
endif()
