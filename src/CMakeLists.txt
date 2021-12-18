cmake_minimum_required(VERSION 3.2)
project(RoutingEngine)

add_subdirectory(Serialization)
add_subdirectory(ContractionHierarchies)
set(SOURCE_FILES main.cpp)

add_executable(RoutingEngine ${SOURCE_FILES})
target_link_libraries(RoutingEngine ContractionHierarchies)
target_link_libraries(RoutingEngine Serialization)
install(TARGETS RoutingEngine DESTINATION ../${ENGINE_INSTALL_BIN_DIR})