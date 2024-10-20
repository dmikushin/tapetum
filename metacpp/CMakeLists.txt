cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ../ThirdParty/rapidjson/include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
