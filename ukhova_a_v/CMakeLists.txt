cmake_minimum_required(VERSION 3.0)

project(ukhova_a_v)



find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Lesson_1 Lesson_1.cpp)
target_link_libraries(Lesson_1 ${OpenCV_LIBS})

add_executable(Lesson_2_Part_1 Lesson_2_Part_1.cpp)
target_link_libraries(Lesson_2_Part_1 ${OpenCV_LIBS})

add_executable(Lesson_2_Part_2 Lesson_2_Part_2.cpp)
target_link_libraries(Lesson_2_Part_2 ${OpenCV_LIBS})

add_executable(Lab_2 Lab_2.cpp)
target_link_libraries(Lab_2 ${OpenCV_LIBS})

add_executable(Lab_3 Lab_3.cpp)
target_link_libraries(Lab_3 ${OpenCV_LIBS})

add_executable(k_r k_r.cpp)
target_link_libraries(k_r ${OpenCV_LIBS})

add_executable(Lab_1 Lab_1.cpp)
target_link_libraries(Lab_1 ${OpenCV_LIBS})
