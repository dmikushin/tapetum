cmake_minimum_required(VERSION 3.5)

project(metacpp LANGUAGES C CXX)

if(WIN32)
    set(OS_NAME "Win")
    set(DISABLE_RTTI_FLAG /GR-)
elseif(APPLE)
    set(OS_NAME "OSX")
    set(DISABLE_RTTI_FLAG -fno-rtti)
elseif(UNIX)
    set(OS_NAME "Linux")
    set(DISABLE_RTTI_FLAG -fno-rtti)
else()
    message(FATAL_ERROR "Operating system not supported")
endif()

if (FALSE)
find_package(LLVM CONFIG REQUIRED)
find_package(Clang CONFIG REQUIRED)
else()
# For Ubuntu with multiple versions of LLVM installed
find_package(LLVM CONFIG REQUIRED PATHS "/usr/lib/llvm-17/lib/cmake" NO_DEFAULT_PATH)
find_package(Clang CONFIG REQUIRED PATHS "/usr/lib/llvm-17/lib/cmake" NO_DEFAULT_PATH)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# MetaCPP automatic reflection
include(cmake/MetaPrebuild.cmake)

add_subdirectory(metacpp)
add_subdirectory(metacpp_cli)

add_subdirectory(example)
