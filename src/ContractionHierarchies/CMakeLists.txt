cmake_minimum_required(VERSION 3.2)

project(ContractionHierarchies)

set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)
set(SOURCE_FILES
        queue.cpp
        graph.cpp
        bidirectionalSearch.cpp
        hierarchyConstructor.cpp
        osmParser.cpp
        weighting.cpp
        lib/pugixml/src/pugixml.cpp)

add_subdirectory(lib/cereal EXCLUDE_FROM_ALL lib/cereal/sandbox)
add_subdirectory(lib/pugixml)

include_directories(lib/cereal/include)
include_directories(lib/pugixml/src)
include_directories(include)

add_library(ContractionHierarchies SHARED STATIC ${SOURCE_FILES})
target_link_libraries(ContractionHierarchies pugixml cereal)
install(TARGETS ContractionHierarchies DESTINATION ../../${ENGINE_INSTALL_LIB_DIR})
install(FILES 
		include/queue.h
        include/graph.h
        include/bidirectionalSearch.h
        include/hierarchyConstructor.h
        include/osmParser.h
        include/weighting.h
		include/serialize.h
        DESTINATION ../../${ENGINE_INSTALL_INCLUDE_DIR})