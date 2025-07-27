cmake_minimum_required(VERSION 3.15)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


add_executable(myapp 
    main.cpp 
    Orderbook.cpp
    OrderbookLevels.cpp
    Order.cpp
)

set(HEADERS
    Direction.h
    OrderbookLevels.h
    Level.h
    OrderType.h
    Orderbook.h
    Using.h
    Order.h
)

target_sources(myapp PRIVATE ${HEADERS})

find_package(GTest QUIET)

if(GTest_FOUND)
    # Tests executable - reuses your source files
    add_executable(tests
        tests/tests.cpp
        Orderbook.cpp
        OrderbookLevels.cpp
        Order.cpp
    )
    
    target_link_libraries(tests GTest::gtest_main)
    
    message(STATUS "Tests enabled - run with './tests'")
else()
    message(STATUS "GTest not found - tests disabled")
endif()
