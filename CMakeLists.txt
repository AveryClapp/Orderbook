cmake_minimum_required(VERSION 3.15)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(boost REQUIRED)

add_executable(myapp 
    src/main.cpp 
    src/core/Orderbook.cpp
    src/core/OrderbookLevels.cpp
)

target_link_libraries(myapp boost::boost)

set(HEADERS
    include/core/Direction.h
    include/core/OrderbookLevels.h
    include/core/Level.h
    include/core/OrderType.h
    include/core/Orderbook.h
    include/core/Using.h
    include/core/Order.h
)

target_sources(myapp PRIVATE ${HEADERS})

find_package(GTest QUIET)
find_package(benchmark QUIET)

file(GLOB_RECURSE TEST_SOURCES 
 "tests/unit/*.cpp"
 "tests/integration/*.cpp"
 "tests/helpers/*.cpp"
)
add_executable(tests
    ${TEST_SOURCES}
    src/core/Orderbook.cpp
    src/core/OrderbookLevels.cpp
)

target_link_libraries(tests GTest::gtest_main)
target_link_libraries(tests benchmark::benchmark)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

