cmake_minimum_required(VERSION 3.5)
project(RoutingEngineBenchmark)

add_subdirectory(lib/nanobench)

include_directories(lib/nanobench/include)

add_executable(RoutingEngineBenchmark src/benchmark.cpp)

target_link_libraries(RoutingEngineBenchmark nanobench)
target_link_libraries(RoutingEngineBenchmark ContractionHierarchies)
target_link_libraries(RoutingEngineBenchmark Catch2)

target_include_directories(RoutingEngineBenchmark PRIVATE Catch2/single_include/catch2)

install(TARGETS RoutingEngineBenchmark DESTINATION ${ENGINE_INSTALL_BIN_DIR})