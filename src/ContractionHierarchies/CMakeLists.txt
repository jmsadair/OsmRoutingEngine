cmake_minimum_required(VERSION 3.2)

project(ContractionHierarchies)

set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)
set(SOURCE_FILES
		src/Graph.cpp
        src/BidirectionalSearch.cpp
		src/HierarchyConstructor.cpp
		src/OsmParser.cpp
		src/Weighting.cpp
        lib/pugixml/src/pugixml.cpp)

add_subdirectory(lib/cereal EXCLUDE_FROM_ALL lib/cereal/sandbox)
add_subdirectory(lib/pugixml)

include_directories(lib/cereal/include)
include_directories(lib/pugixml/src)
include_directories(include)

add_library(ContractionHierarchies SHARED STATIC ${SOURCE_FILES})
target_link_libraries(ContractionHierarchies pugixml cereal)
install(TARGETS ContractionHierarchies DESTINATION ${ENGINE_INSTALL_LIB_DIR})
install(FILES
        include/Queue.h
		include/Graph.h
		include/BidirectionalSearch.h
		include/HierarchyConstructor.h
		include/OsmParser.h
		include/Weighting.h
		include/Serialize.h
        DESTINATION ${ENGINE_HEADERS_DIR})