#
#--- find my standalone GLEW
#
# try default search
MESSAGE(STATUS "try automatic search.")
FIND_PACKAGE(GLEW)
# if it doesn't work, use my standalone library
IF (NOT GLEW_FOUND)
  MESSAGE(STATUS "automatic search failed, try standalone version.")
  FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h
    ${GLEW_ROOT_PATH}/include
    $ENV{GLEWDIR}/include
    /usr/local/include
    /usr/local/X11R6/include
    /usr/X11R6/include
    /usr/X11/include
    /usr/include/X11
    /usr/include
    /opt/X11/include
    /opt/include
  )
  # a special hack for windows
  IF(WIN32)
      IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
          SET(GLEW_LIB_NAME glew32)
          SET(WIN_LIB_PATH ${GLEW_ROOT_PATH}/lib/Release/x64)
      ELSE()
          SET(GLEW_LIB_NAME glew32)
          SET(WIN_LIB_PATH ${GLEW_ROOT_PATH}/lib/Release/Win32)
      ENDIF()
  ELSE()
      # IMPORTANT: uppercase otherwise problem on linux
      SET(GLEW_LIB_NAME GLEW)
  ENDIF()
  # search for library
  FIND_LIBRARY(GLEW_LIBRARIES
      NAMES ${GLEW_LIB_NAME} PATHS
      $ENV{GLEWDIR}/lib
      /usr/local/lib
      /usr/local/X11R6/lib
      /usr/X11R6/lib
      /usr/X11/lib
      /usr/lib/X11
      /usr/lib
      /opt/X11/lib
      /opt/lib
      # FOR UBUNTU 12.04 LTS
      /usr/lib/x86_64-linux-gnu
      # FOR WINDOWS standalone
      ${WIN_LIB_PATH})
  # produce output
  INCLUDE(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(GLEW DEFAULT_MSG
      GLEW_INCLUDE_DIR
      GLEW_LIBRARIES
  )
  MARK_AS_ADVANCED(GLEW_INCLUDE_DIR GLEW_LIBRARIES)
ENDIF()

##############################################################
# redistribute freeglut on Windows
##############################################################

IF (WIN32)
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
      SET(WIN_DLL_PATH ${GLEW_ROOT_PATH}/bin/Release/x64)
  ELSE()
      SET(WIN_DLL_PATH ${GLEW_ROOT_PATH}/bin/Release/Win32)
  ENDIF()
  FIND_FILE(GLEW_DLL
    NAMES glew32.dll
    HINTS ${WIN_DLL_PATH}
  )
  MARK_AS_ADVANCED(GLEW_DLL)
ENDIF()
