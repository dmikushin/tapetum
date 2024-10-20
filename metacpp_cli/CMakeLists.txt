cmake_minimum_required(VERSION 3.13 FATAL_ERROR) # link_directories

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME}_cli ${SRCS})
target_include_directories(${PROJECT_NAME}_cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME}_cli PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME}_cli PRIVATE ../ThirdParty/Mustache)
target_compile_definitions(${PROJECT_NAME}_cli PRIVATE ${LLVM_DEFINITIONS})
target_compile_options(${PROJECT_NAME}_cli PRIVATE -Wno-unused-lambda-capture -Wno-ambiguous-reversed-operator -Wno-unused-parameter -fno-rtti)
target_link_directories(${PROJECT_NAME}_cli PRIVATE ${LLVM_LIB_DIR})
target_link_libraries(${PROJECT_NAME}_cli ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_cli libclang clang-cpp LLVM)

if (WIN32)
    # Annoying lib in Windows
    target_link_libraries(${PROJECT_NAME}_cli PRIVATE version.lib)
endif ()

