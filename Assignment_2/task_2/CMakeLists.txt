cmake_minimum_required(VERSION 2.8.12)
project(Assignment2Task2)
FIND_PACKAGE(OpenCV REQUIRED)


add_compile_options(-std=c++0x)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)

#include_directories(${BULLET_INCLUDE_DIR}
#    ${SOIL_INCLUDE_DIRS}
#    )


SET(MYSOURCE
    task_2.cpp
    )

add_executable(Assignment2Task2 WIN32
    ${MYSOURCE}
    )

target_link_libraries(Assignment2Task2
    ${OpenCV_LIBS}
    )
