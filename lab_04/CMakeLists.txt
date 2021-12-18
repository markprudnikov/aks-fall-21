cmake_minimum_required(VERSION 3.21)
project(lab_04)

set(CMAKE_CXX_STANDARD 14)
add_executable(lab_04 main.cpp)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(lab_04 PUBLIC OpenMP::OpenMP_CXX)
endif()

