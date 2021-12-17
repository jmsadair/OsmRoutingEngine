cmake_minimum_required (VERSION 3.8)

project (OsmRoutingEngine)
add_subdirectory(src/RoutingEngine)
add_executable(OsmRoutingEngine src/main.cpp)

target_include_directories(OsmRoutingEngine
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/dependencies/Catch2/single_include/catch2
        ${PROJECT_SOURCE_DIR}/dependencies/pugixml/src
        ${PROJECT_SOURCE_DIR}/dependencies/cereal/include
        )