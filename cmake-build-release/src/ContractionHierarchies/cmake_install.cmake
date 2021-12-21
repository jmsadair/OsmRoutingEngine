# Install script for directory: C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Program Files/JetBrains/CLion 2021.3/bin/mingw/bin/objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/lib/libContractionHierarchies.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/lib" TYPE STATIC_LIBRARY FILES "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/cmake-build-release/src/ContractionHierarchies/libContractionHierarchies.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/Queue.h;C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/Graph.h;C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/BidirectionalSearch.h;C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/HierarchyConstructor.h;C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/OsmParser.h;C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/Weighting.h;C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/Serialize.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include" TYPE FILE FILES
    "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/Queue.h"
    "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/Graph.h"
    "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/BidirectionalSearch.h"
    "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/HierarchyConstructor.h"
    "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/OsmParser.h"
    "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/Weighting.h"
    "C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/src/ContractionHierarchies/include/Serialize.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/jmsad/Desktop/CPP-Programs/CLion-Projects/OsmRoutingEngine/cmake-build-release/src/ContractionHierarchies/lib/pugixml/cmake_install.cmake")

endif()

