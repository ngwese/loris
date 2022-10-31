include(ExternalProject)
include(FindPkgConfig)


#--------------------------------------------------------------------
# set min version and deployment target -- before project
#--------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Minimum OS X deployment version")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

IF(APPLE)
  SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
ENDIF(APPLE)

#--------------------------------------------------------------------
# project and version
#--------------------------------------------------------------------

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 

project(loris)

#--------------------------------------------------------------------
# Compiler flags
#--------------------------------------------------------------------

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -std=c++17")

  # For now, explicitly disable C++17 alignment feature 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-aligned-new")
elseif(WIN32)
  #untested
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:alignedNew-")
endif()
 
 #--------------------------------------------------------------------
 # Choose library output name
 #--------------------------------------------------------------------
 
 # creates the library loris-debug in debug configuration
 set(LIBRARY_NAME loris$<$<CONFIG:Debug>:-debug>)
 
 
 #--------------------------------------------------------------------
 # Enforce out of source build
 #--------------------------------------------------------------------
 
 if(CMAKE_BINARY_DIR EQUAL CMAKE_SOURCE_DIR)
   message(FATAL_ERROR "Loris requires an out of source build.")
 endif()
 
 #--------------------------------------------------------------------
 # Add include directories
 #--------------------------------------------------------------------
 
 message("\n\n\ncurrent dir: " ${CMAKE_CURRENT_SOURCE_DIR})
 
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
 
 #--------------------------------------------------------------------
 # collect sources and headers
 #--------------------------------------------------------------------
 
 file(GLOB LORIS_SOURCES "src/*.c")
 file(GLOB LORIS_HEADERS_PRIVATE "src/*.h")
 file(GLOB LORIS_HEADERS "include/*.h")
 
 #--------------------------------------------------------------------
 # create and install library
 #--------------------------------------------------------------------
 
 set(target loris)
 
 add_library(${target} STATIC ${LORIS_SOURCES})
 
 
 # send binary output to the current build/bin
 set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

 
 set_target_properties(${target} PROPERTIES
                       OUTPUT_NAME "${LIBRARY_NAME}"
                       VERSION "1.9.0"
                       SOVERSION "1"
                       POSITION_INDEPENDENT_CODE ON
                       FOLDER "loris")
                       
 if(APPLE)
     set_target_properties(${target} PROPERTIES 
         XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym")
 endif()
 
 include(GNUInstallDirs)
 
 install(
     TARGETS ${target}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
 )
 
 #--------------------------------------------------------------------
 # install headers
 #--------------------------------------------------------------------
 
 install(FILES include/loris.h DESTINATION include/loris)


