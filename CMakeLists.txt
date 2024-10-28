cmake_minimum_required(VERSION 3.13 FATAL_ERROR) # link_directories

project(tapetum LANGUAGES C CXX)

# Require that the used C and C++ compilers are the save version of Clang.
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(FATAL_ERROR "C++ compiler must be Clang")
endif()
if(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    message(FATAL_ERROR "C compiler must be Clang")
endif()
if(NOT "${CMAKE_C_COMPILER_VERSION}" STREQUAL "${CMAKE_CXX_COMPILER_VERSION}")
    message(FATAL_ERROR "C and C++ compiler versions must be the same")
endif()

if(WIN32)
    set(DISABLE_RTTI_FLAG /GR-)
elseif(APPLE OR UNIX)
    set(DISABLE_RTTI_FLAG -fno-rtti)
else()
    message(FATAL_ERROR "Operating system not supported")
endif()

string(REGEX MATCH "^([0-9]+)" CXX_COMPILER_VERSION_MAJOR ${CMAKE_CXX_COMPILER_VERSION})

if (FALSE)
find_package(LLVM CONFIG REQUIRED)
find_package(Clang CONFIG REQUIRED)
else()
# For Ubuntu with multiple versions of LLVM installed
find_package(LLVM CONFIG REQUIRED PATHS "/usr/lib/llvm-${CXX_COMPILER_VERSION_MAJOR}/lib/cmake" NO_DEFAULT_PATH)
find_package(Clang CONFIG REQUIRED PATHS "/usr/lib/llvm-${CXX_COMPILER_VERSION_MAJOR}/lib/cmake" NO_DEFAULT_PATH)
endif()

message(STATUS ${CLANG_INCLUDE_DIRS})

# Build a Clang plugin
file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(${PROJECT_NAME}_plugin SHARED ${SRCS})
target_include_directories(${PROJECT_NAME}_plugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME}_plugin PRIVATE ThirdParty/rapidjson/include)
target_include_directories(${PROJECT_NAME}_plugin PRIVATE ThirdParty/mustache)
target_include_directories(${PROJECT_NAME}_plugin PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_compile_features(${PROJECT_NAME}_plugin PUBLIC cxx_std_20)
target_compile_options(${PROJECT_NAME}_plugin PRIVATE ${DISABLE_RTTI_FLAG})
set_target_properties(${PROJECT_NAME}_plugin PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_plugin PROPERTIES PREFIX "")
target_compile_definitions(${PROJECT_NAME}_plugin PRIVATE ${LLVM_DEFINITIONS})
target_compile_options(${PROJECT_NAME}_plugin PRIVATE -Wno-unused-lambda-capture -Wno-ambiguous-reversed-operator -Wno-unused-parameter)
#target_link_directories(${PROJECT_NAME}_plugin PRIVATE ${LLVM_LIB_DIR})
target_link_libraries(${PROJECT_NAME}_plugin libclang clang-cpp LLVM)

# Build an ad-hoc library, which shall add our Clang plugin to the command line
# of the target project.
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} INTERFACE ThirdParty/rapidjson/include)
target_compile_options(${PROJECT_NAME} INTERFACE -fplugin=$<TARGET_FILE:${PROJECT_NAME}_plugin>)

add_subdirectory(example)

