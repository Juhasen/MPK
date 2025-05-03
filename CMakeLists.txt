cmake_minimum_required(VERSION 3.22)
project(mpk)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define an executable target
add_executable(mpk_executable objects/LineFactory.cpp)

# Link the required libraries to the target
target_link_libraries(mpk_executable PRIVATE Ice IceUtil)