#Required version of cmake
cmake_minimum_required (VERSION 2.8)

#Project name. Used to tag resources in cmake. 
project (MapView)

# Also: version number. This will go into the generated header file.
set (Basic_VERSION_MAJOR 0)
set (Basic_VERSION_MINOR 1)

#Option: build tests. Currently has no effect.
option(BUILD_TESTS "Build unit tests." OFF) 

#Turn on verbose output
SET(CMAKE_VERBOSE_MAKEFILE ON)

#Force gcc to output single line errors. 
# This makes it easier for Eclipse to parse and understand each error.
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#Generate a header file to contain configuration settings. This is useful
#  for, e.g., cmake variable which can then be set via the gui. 
configure_file (
  "${PROJECT_SOURCE_DIR}/GenConfig.h.in"
  "${PROJECT_SOURCE_DIR}/GenConfig.h"
)

#Cflags for C++0x support via gcc
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


#Include directories ported over from Eclipse. Can be augmented later with pkgconfig.
include_directories("${ProjDirPath}/../phoenix")
include_directories("${ProjDirPath}/..")
include_directories("${ProjDirPath}/src")

#Link directories
link_directories ("${ProjDirPath}/../phoenix/build")
link_directories ("${ProjDirPath}/../phoenix_ext/build")

#Group as many files as possible together automatically
FILE(GLOB_RECURSE BASIC_CPP *.cpp)

#Create a list of items to remove from the Build.
# Adding main.cpp to this list makes a good starting point, and ensures
# we always have at least one item to remove from BASIC_CPP
#TEMP: Why isn't the previous line working?
FILE(GLOB IgnoreList "src/MapViewer.cpp")

#Remove the "Build" directory
#FILE(GLOB_RECURSE IgnoreTmp "Build/*.c" "Build/*.cpp")
#LIST(APPEND IgnoreList ${IgnoreTmp})

#Remove the "Debug" directories
FILE(GLOB_RECURSE IgnoreTmp "Debug/*.c" "Debug/*.cpp")
LIST(APPEND IgnoreList ${IgnoreTmp})
FILE(GLOB_RECURSE IgnoreTmp "Release/*.c" "Release/*.cpp")
LIST(APPEND IgnoreList ${IgnoreTmp})

#Actually remove these
LIST(REMOVE_ITEM BASIC_CPP ${IgnoreList})

#List of libraries to link
SET(LibraryList X11 sfml-system phoenix_ext phoenix_ext_gtk tinyxml z)

#TODO: pkg-config, parrot_config:
#`pkg-config --cflags gtk+-2.0` `parrot_config embed-cflags`

#TODO: pkg-config, parrot_config:
#`pkg-config --libs gtk+-2.0` `parrot_config embed-ldflags`

#Our output executable and its input sources. List *.c files manually
add_executable(MapView src/MapView.cpp ${BASIC_CPP})

#Libraries to link, ported over from Eclipse.
target_link_libraries (MapView ${LibraryList})




