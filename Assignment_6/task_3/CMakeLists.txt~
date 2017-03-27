project(Assignment6Task2)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
aux_source_directory(. SRC_LIST)


find_package( OpenCV 3 REQUIRED)


include_directories(${OpenCV_INCLUDE_DIRS})

set(project_sources
    task_2.cpp)

add_executable(${PROJECT_NAME} ${project_sources})


target_link_libraries(Assignment6Task2 ${OpenCV_LIBS} )
