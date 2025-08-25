cmake_minimum_required(VERSION 3.15)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG 
    "-g -O0 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual"
)
set(CMAKE_CXX_FLAGS_RELEASE 
    "-O3 -DNDEBUG -march=native -mtune=native -flto -ffast-math -funroll-loops -finline-functions -fno-omit-frame-pointer -g1"
)



# Enable based on build type
if(CMAKE_BUILD_TYPE STREQUAL "AddressSan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_ASAN}")
elseif(CMAKE_BUILD_TYPE STREQUAL "ThreadSan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_TSAN}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost REQUIRED COMPONENTS container)

add_executable(myapp 
    src/main.cpp 
    src/core/Orderbook.cpp
    src/core/OrderbookLevels.cpp
    src/core/OrderPool.cpp
)

target_link_libraries(myapp Boost::boost)

set(HEADERS
    include/core/Direction.h
    include/core/OrderbookLevels.h
    include/core/Level.h
    include/core/OrderType.h
    include/core/Orderbook.h
    include/core/Using.h
    include/core/Order.h
    include/core/OrderPool.h
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
    src/core/OrderPool.cpp
    tests/helpers/test_utils.cpp
)

if(benchmark_FOUND)
    add_executable(benchmarks
        benchmarks/benchmarks.cpp
        src/core/Orderbook.cpp
        src/core/OrderbookLevels.cpp
        src/core/OrderPool.cpp
        tests/helpers/test_utils.cpp
    )
    
    target_link_libraries(benchmarks benchmark::benchmark)
    target_link_libraries(benchmarks Boost::boost)
    target_include_directories(benchmarks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    
    # Add perf profiling target
    add_custom_target(profile
        COMMAND echo "Starting perf profiling..."
        COMMAND perf record -g --call-graph=dwarf -o perf.data $<TARGET_FILE:benchmarks>
        COMMAND echo "Profiling complete. Run 'perf report' to view results."
        DEPENDS benchmarks
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
target_link_libraries(tests GTest::gtest_main)
target_link_libraries(tests benchmark::benchmark)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

