# CMakeLists.txt
#
# Copyright 2021 by
# Jordi Espada
#
# Say
#
#   cmake CMakeLists.txt
#
# to create a Makefile that builds a static version of the library.  For a
# dynamic library, use
#
#   cmake CMakeLists.txt -DBUILD_SHARED_LIBS:BOOL=true
#
#
# instead.  Please refer to the cmake manual for further options, in
# particular, how to modify compilation and linking parameters.
#

cmake_minimum_required(VERSION 3.15)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  MESSAGE(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the 'zetscript' source code and call cmake from there. For instance,\nmkdir build\ncd build\ncmake ..")
endif()

cmake_policy(SET CMP0091 NEW)	# To enable MSVC_RUNTIME_LIBRARY. It has effect only when policy CMP0091 is set to NEW prior to the first project() 
project(zetscript)

MESSAGE("-- binary dir:" ${PROJECT_BINARY_DIR} )
MESSAGE("-- source dir:" ${PROJECT_SOURCE_DIR} )

# Recurse into the "src" subdirectories.
add_subdirectory (src)

