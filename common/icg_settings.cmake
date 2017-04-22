# Build configuration file for "Intro to Graphics"
# Copyright (C) 2014 - LGG EPFL
#
#--- To understand its content: 
#   http://www.cmake.org/cmake/help/syntax.html
#   http://www.cmake.org/Wiki/CMake_Useful_Variables
#

#--- This is how you show a status message in the build system
message(STATUS "Intro to Graphics - Loading Common Configuration")

#--- Tell CMake he'll be able to include ".cmake" configurations 
# files in the folder where the current file is located
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

#--- Configures compiler for C++11
if(EXISTS "/usr/bin/icc")
    #--- If intel compiler is installed use it (ICC is already c++11 compliant)
    message(STATUS "Using ICC Intel Compiler.")
    set(CMAKE_CC_COMPILER "/usr/bin/icc")
    set(CMAKE_CXX_COMPILER "/usr/bin/icc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(UNIX)
    if(NOT APPLE)
        # Enable c++11 for GCC 
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        # Clang in OSX supports partially c++11 through extensions
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-extensions")        
    endif()
elseif(WIN32)    
    # MSVC12 supports c++11 natively
endif()

#--- CMake extension to load GLFW
find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
add_definitions(${GLFW_DEFINITIONS})
if(NOT GLFW_FOUND)
    message(ERROR " GLFW not found!")
endif()

#--- GLEW (STATICALLY LINKED)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(-DGLEW_STATIC)
# message(STATUS "HERE " ${GLEW_LIBRARIES})

#--- Wraps file deployments
macro(target_deploy_file MYTARGET FILEPATH)
    #--- FALSE: always use the "copy" just to avoid cross platform problems
    if(FALSE)
        #--- simple symbolic link
        execute_process(COMMAND ln -f -s ${CMAKE_SOURCE_DIR}/${FILEPATH} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    else()
        #--- windows likes to make my life difficult, copy them
        get_filename_component(FILENAME ${FILEPATH} NAME_WE)
        set(TARGET_NAME deployfile_${MYTARGET}_${FILENAME})
        set(TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR})
        add_custom_target( # always out of date
            ${TARGET_NAME} ALL # always built
            SOURCES ${FILEPATH} # adds file to project manager
            COMMAND ${CMAKE_COMMAND} -E copy ${FILEPATH} ${TARGET_DIR}
            DEPENDS ${FILEPATH}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endmacro()

#--- OPENGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND) 
    message(ERROR " OPENGL not found!")
endif()

#--- On UNIX|APPLE you can do "make update_opengp" to update
if(CMAKE_HOST_UNIX)
    add_custom_target( update_opengp
        COMMAND rm -rf OpenGP
        COMMAND svn checkout https://github.com/OpenGP/OpenGP/trunk/src/OpenGP
        COMMAND rm -rf OpenGP/.svn
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/external/OpenGP/include
        COMMENT "Updating OpenGP ${CMAKE_SOURCE_DIR}")
endif()

#--- Make headers in common directory visible in the IDE
file(GLOB_RECURSE COMMON_DIR_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")
add_custom_target(common_headers SOURCES ${COMMON_DIR_HEADERS})

#--- OPENGP
find_package(OpenGP REQUIRED)
include_directories(${OpenGP_INCLUDE_DIRS})
add_definitions(-DHEADERONLY)
add_definitions(-DUSE_EIGEN)
if(NOT OPENGP_FOUND)
    message(ERROR " OPENGP not found!")
endif()

#--- Eigen3
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})
if(NOT EIGEN3_FOUND)
    message(ERROR " EIGEN not found!")
endif() 

#--- Common headers/libraries for all the exercises
include_directories(${CMAKE_CURRENT_LIST_DIR})
SET(COMMON_LIBS ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})

#--- OpenCV (Optional!!)
find_package(PkgConfig)
if(PKGCONFIG_FOUND)
    pkg_check_modules(OPENCV opencv)
    list(APPEND COMMON_LIBS ${OPENCV_LDFLAGS})
    include_directories(${OPENCV_INCLUDE_DIRS})
    if(OPENCV_FOUND)
        add_definitions(-DWITH_OPENCV)
    endif()
endif()

#--- AntTweakBar
find_package(AntTweakBar)
if(ANTTWEAKBAR_FOUND)
    include_directories(${ANTTWEAKBAR_INCLUDE_DIR})
    list(APPEND COMMON_LIBS ${ANTTWEAKBAR_LIBRARY})
    add_definitions(-DWITH_ANTTWEAKBAR)
endif()

#--- X11 (required by AntTweakBar)
if(UNIX AND NOT APPLE)
    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "Cannot found X11 on linux?")
    endif()
    list(APPEND COMMON_LIBS ${X11_LIBRARIES})
endif()

#--- OBSOLETE: as now shaders are deployed in .h files
# Each exercise outputs a binary to the source folder of each exercise
# otherwise, you'd have to move the shaders to the deploy folder!!!!!!
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#--- OBSOLETE: use global deploy folder
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/deploy)

#--- TODO: CMake extension to specify launch configs for IDEs
# include(CreateLaunchers)
# create_target_launcher(ex2 WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
