cmake_minimum_required(VERSION 3.8)
project(Parsing)
set(SOURCE_FILES src/OsmParser.cpp src/weighting.cpp lib/pugixml/src/pugixml.cpp)
add_subdirectory(lib/pugixml)
add_library(Parsing SHARED STATIC ${SOURCE_FILES})
target_include_directories(Parsing PUBLIC include lib/pugixml/src)
target_link_libraries(Parsing PRIVATE ContractionHierarchies)
install(TARGETS Parsing DESTINATION ${ENGINE_INSTALL_LIB_DIR})
install(FILES include/OsmParser.h include/Weighting.h DESTINATION ${PARSING_HEADERS_DIR})