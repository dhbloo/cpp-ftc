cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
cmake_policy(VERSION 3.12)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(DEFAULT_BUILD_EXAMPLES ON)
    set(DEFAULT_BUILD_TESTS ON)
endif()

option(BUILD_EXAMPLES "Build example programs" ${DEFAULT_BUILD_EXAMPLES})
option(BUILD_TESTS "Build test programs" ${DEFAULT_BUILD_TESTS})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
        STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(
    CppFTC
    VERSION "0.1.1"
    DESCRIPTION "C++ fundamental template collection"
    LANGUAGES CXX
)

add_library(FTC INTERFACE)
add_library(FTC::FTC ALIAS FTC)

target_include_directories(FTC
    INTERFACE ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(FTC
    INTERFACE cxx_std_17
)

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
