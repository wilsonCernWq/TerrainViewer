#
#--- Libraries Setups
#
# Modified based on the course project from EPFL ICG course that I have taken
# This is used for configure the environment with CMAKE
#
# Build configuration file for "Intro to Graphics"
# Copyright (C) 2014 - LGG EPFL
#
#--- To understand its content:
#   http://www.cmake.org/cmake/help/syntax.html
#   http://www.cmake.org/Wiki/CMake_Useful_Variables
#
#--- Interface
#   This module wil define two global variables
#--- This is how you show a status message in the build system
MESSAGE(STATUS "Interactive Computer Graphics - Loading Common Configuration")
#
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
#
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/configcxx.cmake)
#
# Initialize library pathes and dll path
SET(COMMON_LIBS "") # those will be link for each project
SET(COMMON_DLLS "") # those files will be copyed to the executable folder
SET(COMMON_DEFINITIONS "")
#
#--- OPENGL
FIND_PACKAGE(OpenGL REQUIRED)
if(OPENGL_FOUND)
  MESSAGE(STATUS " OPENGL found!  ${OPENGL_LIBRARIES}")
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
  LIST(APPEND COMMON_LIBS ${OPENGL_LIBRARIES})
  if (OPENGL_GLU_FOUND)
    MESSAGE(STATUS " GLU found!")
  else()
    MESSAGE(ERROR " GLU not found!")
  endif()
else()
  MESSAGE(ERROR " OPENGL not found!")
endif()
#
#--- CMake extension to load GLUT
SET(GLUT_ROOT_PATH "${PROJECT_SOURCE_DIR}/external/freeglut")
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glut.cmake)
if(GLUT_FOUND)
    INCLUDE_DIRECTORIES(library ${GLUT_INCLUDE_DIR})
    LIST(APPEND COMMON_LIBS ${GLUT_LIBRARIES})
    LIST(APPEND COMMON_DLLS ${GLUT_DLL})
else()
    MESSAGE(FATAL_ERROR " GLUT not found!")
endif()
#
#--- GLEW
SET(GLEW_ROOT_PATH "${PROJECT_SOURCE_DIR}/external/glew")
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glew.cmake)
if(GLUT_FOUND)
    INCLUDE_DIRECTORIES(library ${GLEW_INCLUDE_DIR})
    LIST(APPEND COMMON_LIBS ${GLEW_LIBRARIES})
    LIST(APPEND COMMON_DLLS ${GLEW_DLL})
else()
    MESSAGE(FATAL_ERROR " GLEW not found!")
endif()
#
#--- Eigen3 (optional)
FIND_PACKAGE(EIGEN)
IF (EIGEN_FOUND)
  INCLUDE_DIRECTORIES(library ${EIGEN_INCLUDE_DIR})
  ADD_DEFINITIONS(-DWITH_EIGEN)
  MESSAGE(STATUS " EIGEN found!")
ELSE()
  MESSAGE(WARNING " EIGEN not found!")
ENDIF()
#
#--- GLM (optional)
FIND_PACKAGE(GLM)
IF (GLM_FOUND)
  INCLUDE_DIRECTORIES(library ${GLM_INCLUDE_DIR})
  ADD_DEFINITIONS(-DWITH_GLM)
  MESSAGE(STATUS " GLM found!")
ELSE()
  MESSAGE(WARNING " GLM not found!")
ENDIF()
#
#--- add cyCodeBase (submodule)
#   https://github.com/cemyuksel/cyCodeBase.git
INCLUDE_DIRECTORIES(library ${PROJECT_SOURCE_DIR}/external/cyCodeBase)
#
#
#--- lodePNG
#   http://lodev.org/lodepng/
FIND_PACKAGE(LodePNG REQUIRED)
INCLUDE_DIRECTORIES(library ${LodePNG_INCLUDE_DIR})
LIST(APPEND COMMON_LIBS ${LodePNG_LIBRARIES})
#
#
#--- ImGui
FIND_PACKAGE(ImGui REQUIRED)
INCLUDE_DIRECTORIES(library ${IMGUI_INCLUDE_DIR})
LIST(APPEND COMMON_LIBS ${IMGUI_LIBRARIES})
#
#
#--- add boost external headers
# INCLUDE_DIRECTORIES(library "${PROJECT_SOURCE_DIR}/external/boost")
#
#--- Make headers in common directory visible in the IDE
# FILE(GLOB_RECURSE COMMON_DIR_HEADERS "${PROJECT_SOURCE_DIR}/common/*.h")
# INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/common)
# MARK_AS_ADVANCED(COMMON_DIR_HEADERS)
