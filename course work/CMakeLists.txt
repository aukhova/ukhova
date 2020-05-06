cmake_minimum_required(VERSION 3.0)

project(C_W)



find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(C_W C_W.cpp)
target_link_libraries(C_W ${OpenCV_LIBS})
