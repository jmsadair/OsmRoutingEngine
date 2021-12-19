cmake_minimum_required(VERSION 3.2)
project(ContractionHierarchiesTests)

add_subdirectory(lib/Catch2)
include_directories(${ENGINE_HEADERS_DIR})
include_directories(lib/Catch2/single_include/catch2)

set(SOURCE_FILES src/test.cpp)

add_executable(ContractionHierarchiesTests ${SOURCE_FILES})
target_link_libraries(ContractionHierarchiesTests ContractionHierarchies Catch2)
install(TARGETS ContractionHierarchiesTests DESTINATION bin)