
cmake_minimum_required (VERSION 3.8)

project ("OsmRoutingEngine")

set(SOURCES
    examples/example1.cpp
    src/queue.cpp
    src/graph.cpp
    src/osmParser.cpp
    src/weighting.cpp
    src/serialize.cpp
    src/hierarchyConstructor.cpp
    src/bidirectionalSearch.cpp
    tests/tests.cpp
    dependencies/pugixml/src/pugixml.cpp
)

add_executable (OsmRoutingEngine ${SOURCES})

target_include_directories(OsmRoutingEngine
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/dependencies/pugixml/src
        ${PROJECT_SOURCE_DIR}/dependencies/cereal/include
)