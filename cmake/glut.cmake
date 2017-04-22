# Module imported for searching freeglut
## ======================================================================== ##
## Copyright 2009-2016 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

# FIND_PACKAGE(OpenGL REQUIRED)

IF (APPLE)
  FIND_PACKAGE(GLUT REQUIRED)
  IF (NOT GLUT_FOUND)
    # These values for Apple could probably do with improvement.
    find_path(GLUT_INCLUDE_DIR glut.h
      /System/Library/Frameworks/GLUT.framework/Versions/A/Headers
      ${OPENGL_LIBRARY_DIR}
    )
    set(GLUT_glut_LIBRARY "-framework Glut" CACHE STRING "GLUT library for OSX")
    set(GLUT_cocoa_LIBRARY "-framework Cocoa" CACHE STRING "Cocoa framework for OSX")
  ENDIF ()
ELSEIF (WIN32)
  # detect and select x64
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH /x64)
  ENDIF()
  # Clean Cache !!! ...
  FIND_PATH(GLUT_INCLUDE_DIR NAMES GL/glut.h
    PATHS  ${GLUT_ROOT_PATH}/include )
  FIND_LIBRARY( GLUT_glut_LIBRARY NAMES glut glut32 freeglut
    PATHS
    ${OPENGL_LIBRARY_DIR}
    ${GLUT_ROOT_PATH}/lib${ARCH}
    ${GLUT_ROOT_PATH}/Release
    )
  SET(GLUT_LIBRARIES ${GLUT_glut_LIBRARY})
  MARK_AS_ADVANCED(GLUT_INCLUDE_DIR GLUT_glut_LIBRARY)
ELSE()
  FIND_PACKAGE(GLUT)
  IF (GLUT_FOUND)
    SET(GLUT_LIBRARIES glut GLU m)
  ELSE()
    # try again with method from
    #    https://github.com/tlorach/OpenGLText/blob/master/cmake/FindGLUT.cmake
    MESSAGE(STATUS "Automatic searching for GLUT failed, try again ...")
    FIND_PATH( GLUT_INCLUDE_DIR GL/glut.h
        ${GLUT_LOCATION}/include
        $ENV{GLUT_LOCATION}/include
        /usr/include
        /usr/include/GL
        /usr/local/include
        /usr/openwin/share/include
        /usr/openwin/include
        /usr/X11R6/include
        /usr/include/X11
        /opt/graphics/OpenGL/include
        /opt/graphics/OpenGL/contrib/libglut
    )
    FIND_LIBRARY( GLUT_glut_LIBRARY glut
        ${GLUT_LOCATION}/lib
        $ENV{GLUT_LOCATION}/lib
        /usr/lib
        /usr/local/lib
        /usr/openwin/lib
        /usr/X11R6/lib
    )
    FIND_LIBRARY( GLUT_Xi_LIBRARY Xi
        ${GLUT_LOCATION}/lib
        $ENV{GLUT_LOCATION}/lib
        /usr/lib
        /usr/local/lib
        /usr/openwin/lib
        /usr/X11R6/lib
    )
    FIND_LIBRARY( GLUT_Xmu_LIBRARY Xmu
        ${GLUT_LOCATION}/lib
        $ENV{GLUT_LOCATION}/lib
        /usr/lib
        /usr/local/lib
        /usr/openwin/lib
        /usr/X11R6/lib
    )
    if(GLUT_INCLUDE_DIR AND GLUT_glut_LIBRARY)
      # Is -lXi and -lXmu required on all platforms that have it?
      # If not, we need some way to figure out what platform we are on.
      set( GLUT_LIBRARIES
        ${GLUT_glut_LIBRARY}
        ${GLUT_Xmu_LIBRARY}
        ${GLUT_Xi_LIBRARY}
        ${GLUT_cocoa_LIBRARY}
      )
      MARK_AS_ADVANCED(GLUT_INCLUDE_DIR
        GLUT_glut_LIBRARY
        GLUT_Xmu_LIBRARY
        GLUT_Xi_LIBRARY
        GLUT_cocoa_LIBRARY
      )
      set( GLUT_FOUND "YES" )
      set (GLUT_LIBRARY ${GLUT_LIBRARIES})
      set (GLUT_INCLUDE_PATH ${GLUT_INCLUDE_DIR})
    endif()
  ENDIF()
ENDIF()

# produce output
INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLUT DEFAULT_MSG
    GLUT_INCLUDE_DIR
    GLUT_LIBRARIES
)

##############################################################
# redistribute freeglut on Windows
##############################################################

IF (WIN32)
  FIND_FILE(GLUT_DLL
    NAMES freeglut.dll
    HINTS ${GLUT_INCLUDE_DIR}/../bin/${ARCH} ${FREEGLUT_ROOT_PATH}/bin/${ARCH}
  )
  MARK_AS_ADVANCED(GLUT_DLL)
ENDIF()
