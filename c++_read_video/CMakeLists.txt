cmake_minimum_required(VERSION 2.8.12)
project(c++_read_video)
FIND_PACKAGE(OpenCV REQUIRED)


add_compile_options(-std=c++0x)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)

#include_directories(${BULLET_INCLUDE_DIR}
#    ${SOIL_INCLUDE_DIRS}
#    )


SET(MYSOURCE
    main.cpp
    )

add_executable(c++_read_video WIN32
    ${MYSOURCE}
    )

target_link_libraries(c++_read_video
    ${OpenCV_LIBS}
    )



