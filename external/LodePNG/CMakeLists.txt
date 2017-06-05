GET_FILENAME_COMPONENT(PROJECTNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(${PROJECTNAME})

IF (WIN32)
  ADD_LIBRARY(lodepng
     lodepng.cpp
     lodepng.h
     )

  ADD_LIBRARY(lodepng_util
    lodepng_util.cpp
    lodepng_util.h
    )
ELSE ()
  ADD_LIBRARY(lodepng SHARED
     lodepng.cpp
     lodepng.h
     )

  ADD_LIBRARY(lodepng_util SHARED
    lodepng_util.cpp
    lodepng_util.h
    )
ENDIF()

ADD_EXECUTABLE(pngdetail
  pngdetail.cpp
  lodepng
  lodepng_util
)

ADD_EXECUTABLE(lodepng_unittest
  lodepng_unittest.cpp
  lodepng
  lodepng_util
)
