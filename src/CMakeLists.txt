cmake_minimum_required(VERSION 3.10) #Set minimum CMake version to run this

project(gummy VERSION 0.1)

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17) #Set the C++ Standard
set(CMAKE_BUILD_TYPE Debug)

add_library(matrix Matrix.cpp Matrix.hpp)
add_library(dts_search DTSSearch.cpp DTSSearch.hpp)
add_library(mts_search MSTSearch.cpp MSTSearch.hpp)
add_library(simulated_annealing SimAnneal.cpp SimAnneal.hpp)

add_executable(gummy qubo.cpp) #generate executable
target_link_libraries(dts_search matrix)
target_link_libraries(gummy matrix dts_search mts_search simulated_annealing)
target_include_directories(gummy PUBLIC "${PROJECT_BINARY_DIR}")
