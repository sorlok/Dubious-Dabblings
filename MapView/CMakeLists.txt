#Required version of cmake
cmake_minimum_required (VERSION 2.8)

#Project name. Used to tag resources in cmake. 
project (MapView)

# Also: version number. This will go into the generated header file.
set (Basic_VERSION_MAJOR 0)
set (Basic_VERSION_MINOR 1)

#Option: build tests. Currently has no effect.
option(BUILD_TESTS "Build unit tests." OFF)

#Option: build shared library.
option(BUILD_SHARED_LIBRARY "Build the shared library." OFF)  

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
  "${PROJECT_SOURCE_DIR}/src/GenConfig.h.in"
  "${PROJECT_SOURCE_DIR}/src/GenConfig.h"
)

#Cflags for C++0x support via gcc
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


#Include directories ported over from Eclipse. Can be augmented later with pkgconfig.
include_directories("${PROJECT_SOURCE_DIR}/../phoenix")
include_directories("${PROJECT_SOURCE_DIR}/..")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("/usr/local/include")

#Link directories
link_directories ("${PROJECT_SOURCE_DIR}/../phoenix/build")
link_directories ("${PROJECT_SOURCE_DIR}/../phoenix_ext/build")

#Group as many files as possible together automatically
FILE(GLOB_RECURSE BASIC_CPP *.cpp)

#Create a list of items to remove from the Build.
# Adding main.cpp to this list makes a good starting point, and ensures
# we always have at least one item to remove from BASIC_CPP
FILE(GLOB IgnoreList "src/MapViewer.cpp")

#Remove the library too...
FILE(GLOB IgnoreTmp "src/SFML_Engine.cpp")
LIST(APPEND IgnoreList ${IgnoreTmp})

#And its test file
FILE(GLOB IgnoreTmp "src/SFML_Engine_Test.cpp")
LIST(APPEND IgnoreList ${IgnoreTmp})

#Remove the "Debug" directories
FILE(GLOB_RECURSE IgnoreTmp "Debug/*.c" "Debug/*.cpp")
LIST(APPEND IgnoreList ${IgnoreTmp})
FILE(GLOB_RECURSE IgnoreTmp "Release/*.c" "Release/*.cpp")
LIST(APPEND IgnoreList ${IgnoreTmp})
FILE(GLOB_RECURSE IgnoreTmp "Shared/*.c" "Shared/*.cpp")
LIST(APPEND IgnoreList ${IgnoreTmp})

#Actually remove these
LIST(REMOVE_ITEM BASIC_CPP ${IgnoreList})

#List of libraries to link 
SET(LibraryList sfml-system tinyxml z)

#Libraries change depending on what we're building
IF (${BUILD_SHARED_LIBRARY} MATCHES "OFF")
  LIST(APPEND LibraryList phoenix_ext phoenix_ext_gtk)
ELSE()
  LIST(APPEND LibraryList sfml-graphics sfml-window)
ENDIF (${BUILD_SHARED_LIBRARY} MATCHES "OFF")

#Add Gtk-2 stuff
find_package(GTK2 REQUIRED)
include_directories(${GTK2_INCLUDE_DIRS})
LIST(APPEND LibraryList ${GTK2_LIBRARIES})

#Add X11
LIST(APPEND LibraryList "X11")

#Add Parrot stuff
EXEC_PROGRAM(parrot_config ARGS embed-cflags OUTPUT_VARIABLE PARROT_CFLAGS)
EXEC_PROGRAM(parrot_config ARGS embed-ldflags OUTPUT_VARIABLE PARROT_LDFLAGS)

#Some hacking to get this to work right.
STRING(REGEX REPLACE "[-][L]([^ ;])+" "" PARROT_LDFLAGS "${PARROT_LDFLAGS}" )
STRING(REGEX REPLACE "[-][l]" "" PARROT_LDFLAGS "${PARROT_LDFLAGS}" )
STRING(REGEX REPLACE "^ +" "" PARROT_LDFLAGS "${PARROT_LDFLAGS}" )
STRING(REGEX REPLACE " +$" "" PARROT_LDFLAGS "${PARROT_LDFLAGS}" )
STRING(REGEX REPLACE " +" ";" PARROT_LDFLAGS "${PARROT_LDFLAGS}" )

#todo: Should try to make a FindParrot file.
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PARROT_CFLAGS}")
  LIST(APPEND LibraryList ${PARROT_LDFLAGS})
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#Build shared library, or exe?
IF (${BUILD_SHARED_LIBRARY} MATCHES "OFF")
  #Our output executable and its input sources. List *.c files manually
  add_executable(MapView src/MapViewer.cpp ${BASIC_CPP})
    
  #Libraries to link, ported over from Eclipse.
  target_link_libraries (MapView ${LibraryList})
ELSE()
  #Our shared lib
  ADD_LIBRARY(sfml_engine SHARED src/SFML_Engine.cpp ${BASIC_CPP})
  target_link_libraries (sfml_engine ${LibraryList})
  
  #We build a small executable with the library, just to make sure linking works.
  add_executable(EngineTest src/SFML_Engine_Test.cpp ${BASIC_CPP})
  target_link_libraries (EngineTest ${LibraryList} sfml_engine)
ENDIF (${BUILD_SHARED_LIBRARY} MATCHES "OFF")





