cmake_minimum_required(VERSION 3.2)
project(ContractionHierarchies)
set(SOURCE_FILES
        queue.cpp
        graph.cpp
        bidirectionalSearch.cpp
        hierarchyConstructor.cpp
        osmParser.cpp
        weighting.cpp
        ../../dependencies/pugixml/src/pugixml.cpp
        )
add_library(ContractionHierarchies SHARED STATIC ${SOURCE_FILES})

install(TARGETS ContractionHierarchies DESTINATION ../../${ENGINE_INSTALL_LIB_DIR})
install(FILES include/queue.h
              include/graph.h
              include/bidirectionalSearch.h
              include/hierarchyConstructor.h
              include/osmParser.h
              include/weighting.h
        DESTINATION ../../${ENGINE_INSTALL_INCLUDE_DIR})