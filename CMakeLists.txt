cmake_minimum_required(VERSION 3.2)

project(OsmRoutingEngine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(ENGINE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/ContractionHierarchies/include)
set(ENGINE_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(ENGINE_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

include_directories(${ENGINE_HEADERS_DIR})

add_subdirectory(src)
add_subdirectory(tests)
