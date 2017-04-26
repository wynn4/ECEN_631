project(Assignment8Task1)
cmake_minimum_required(VERSION 2.8)
add_compile_options(-std=c++0x)
set(CMAKE_CXX_STANDARD 11)
aux_source_directory(. SRC_LIST)


find_package(OpenCV 3 REQUIRED)


include_directories(${OpenCV_INCLUDE_DIRS})

set(project_sources
    task_1.cpp)

add_executable(${PROJECT_NAME} ${project_sources})


target_link_libraries(Assignment8Task1 ${OpenCV_LIBS} )
