cmake_minimum_required(VERSION 3.15)
project(MyProject)

#find_package(Boost REQUIRED)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(myapp orderbook.cpp)
