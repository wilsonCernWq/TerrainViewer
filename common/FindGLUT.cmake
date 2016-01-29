# Locate the GLUT library (version 2.0)
# This module defines the following variables:
# GLUT_LIBRARIES, the name of the library;
# GLUT_INCLUDE_DIRS, where to find GLUT include files;
# GLUT_FOUND, true if library path was resolved;
# GLUT_DLLPATH, where to find GLUT dynamic libraries.
#
# Usage example to compile an "executable" target to the glut library:
#
# FIND_PACKAGE (GLUT REQUIRED)
# INCLUDE_DIRECTORIES (${GLUT_INCLUDE_DIRS})
# ADD_EXECUTABLE (executable ${YOUR_EXECUTABLE_SRCS})
# TARGET_LINK_LIBRARIES (executable ${GLUT_LIBRARIES})
#
# TODO:
# Lookup for windows
# Allow the user to select to link to a shared library or to a static library.
#

SET(BITS "")

IF (WIN32)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(BITS "x64")
	SET(LIBPATH ${CMAKE_SOURCE_DIR}/external/glut/lib/x64)
        SET(DLLPATH ${CMAKE_SOURCE_DIR}/external/glut/bin/x64)
        message(STATUS "64 bits system")
    ELSE()
	SET(LIBPATH ${CMAKE_SOURCE_DIR}/external/glut/lib)
        SET(DLLPATH ${CMAKE_SOURCE_DIR}/external/glut/bin)
        message(STATUS "32 bits system")
    ENDIF()
ELSE()
    message(STATUS "Don't know what to do in non windows system")
ENDIF()

FIND_PATH(GLUT_INCLUDE_DIRS GL/glut.h PATHS
    ${CMAKE_SOURCE_DIR}/external/glut/include)

FIND_LIBRARY(GLUT_LIBRARIES NAMES freeglut PATHS
    ${LIBPATH})

FIND_FILE(GLUT_DLLPATH NAMES freeglut.dll PATHS
    ${DLLPATH})

SET(GLUT_FOUND "NO")

IF(NOT GLUT_INCLUDE_DIRS)
    message(ERROR " Found GLUT Include Path")
ENDIF()

IF(NOT GLUT_LIBRARIES)
    message(ERROR " No Found GLUT Library")
ENDIF()

IF(NOT GLUT_DLLPATH)
    message(ERROR " No Found GLUT Dynamic Library")
    message(STATUS "Found GLUT dll: ${GLUT_DLLPATH}")
ENDIF()

IF(GLUT_LIBRARIES AND GLUT_INCLUDE_DIRS AND GLUT_DLLPATH)
    SET(GLUT_FOUND "YES")
ENDIF()
