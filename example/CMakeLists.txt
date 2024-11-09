cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}_example main.cpp)
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})
# TODO Does not trigger rebuild of the example programs
add_dependencies(${PROJECT_NAME}_example ${PROJECT_NAME}_plugin)

