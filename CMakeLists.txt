cmake_minimum_required(VERSION 3.22)

#-----------------------------------------------------
# project definition
#-----------------------------------------------------

project(
        project_name
        VERSION 0.1.0
        LANGUAGES CXX
)

#-----------------------------------------------------
# global C++-settings
#-----------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#-----------------------------------------------------
# options (feature-toggles)
#-----------------------------------------------------

option(ENABLE_TESTS "Build tests" ON)
option(ENABLE_GUI "Build Qt GUI" OFF)
option(ENABLE_CLI "Build CLI application" OFF)

#-----------------------------------------------------
# warnings & compiler-flags
#-----------------------------------------------------

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if (ENABLE_CLI AND ENABLE_GUI)
    message(FATAL_ERROR "ENABLE_CLI and ENABLE_GUI cannot be enabled at the same time")
endif()

if (NOT (ENABLE_CLI OR ENABLE_GUI))
    message(FATAL_ERROR "Either ENABLE_CLI or ENABLE_GUI must be enabled")
endif()
#-----------------------------------------------------
# subdirectories
#-----------------------------------------------------

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/cmake
)

add_subdirectory(src)

if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
