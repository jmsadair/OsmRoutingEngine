cmake_minimum_required(VERSION 3.8)

project(OsmRoutingEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(CH_HEADERS_DIR RoutingEngine/ContractionHierarchies/include)
set(PARSING_HEADERS_DIR RoutingEngine/Parsing/include)
set(ENGINE_HEADERS_DIR RoutingEngine)
set(ENGINE_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(ENGINE_INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR})

include_directories(${CH_HEADERS_DIR})
include_directories(${PARSING_HEADERS_DIR})
include_directories(${ENGINE_HEADERS_DIR})

add_subdirectory(RoutingEngine)
add_subdirectory(tests)
add_subdirectory(benchmark)
add_subdirectory(examples)
add_subdirectory(bindings)

