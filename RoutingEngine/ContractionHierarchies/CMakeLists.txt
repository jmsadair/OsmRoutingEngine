cmake_minimum_required(VERSION 3.8)

project(ContractionHierarchies)

set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)
set(SOURCE_FILES
		src/Graph.cpp
		src/BidirectionalSearch.cpp
		src/HierarchyConstructor.cpp
		)
add_subdirectory(lib/cereal EXCLUDE_FROM_ALL lib/cereal/sandbox)
add_library(ContractionHierarchies SHARED STATIC ${SOURCE_FILES})
target_link_libraries(ContractionHierarchies PUBLIC cereal)
target_include_directories(ContractionHierarchies PUBLIC lib/cereal/include include)
install(TARGETS ContractionHierarchies DESTINATION ${ENGINE_INSTALL_LIB_DIR})
install(FILES
		include/Queue.h
		include/Graph.h
		include/BidirectionalSearch.h
		include/HierarchyConstructor.h
		include/Serialize.h
		DESTINATION ${CH_HEADERS_DIR})
