#
#--- to include LodePNG library
#
SET(ARCH "")
IF(WIN32)
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH /x64)
  ENDIF()
ENDIF()

SET(LodePNG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/external/LodePNG)

IF (WIN32)
  FIND_LIBRARY(LodePNG_release_LIBRARIES NAMES lodepng
    PATHS
    ${PROJECT_SOURCE_DIR}/external/LodePNG/lib/Release${ARCH}
    )
  FIND_LIBRARY(LodePNG_util_release_LIBRARIES NAMES lodepng_util
    PATHS
    ${PROJECT_SOURCE_DIR}/external/LodePNG/lib/Release${ARCH}
    )
  FIND_LIBRARY(LodePNG_debug_LIBRARIES NAMES lodepng
    PATHS
    ${PROJECT_SOURCE_DIR}/external/LodePNG/lib/Debug${ARCH}
    )
  FIND_LIBRARY(LodePNG_util_debug_LIBRARIES NAMES lodepng_util
    PATHS
    ${PROJECT_SOURCE_DIR}/external/LodePNG/lib/Debug${ARCH}
    )
  SET(LodePNG_LIBRARIES
    optimized ${LodePNG_release_LIBRARIES} ${LodePNG_util_release_LIBRARIES}
    debug ${LodePNG_debug_LIBRARIES} ${LodePNG_util_debug_LIBRARIES}
    )
ELSE()
  FIND_LIBRARY(LodePNG_release_LIBRARIES NAMES lodepng
    PATHS
    ${PROJECT_SOURCE_DIR}/external/LodePNG/lib/Release${ARCH}
    )
  FIND_LIBRARY(LodePNG_util_release_LIBRARIES NAMES lodepng_util
    PATHS
    ${PROJECT_SOURCE_DIR}/external/LodePNG/lib/Release${ARCH}
    )
  SET(LodePNG_LIBRARIES
    ${LodePNG_release_LIBRARIES} ${LodePNG_util_release_LIBRARIES}    
    )
ENDIF()

find_package_handle_standard_args(LodePNG
  DEFAULT_MSG
  LodePNG_INCLUDE_DIR
  LodePNG_LIBRARIES
  )

MARK_AS_ADVANCED(
  LodePNG_release_LIBRARIES
  LodePNG_util_release_LIBRARIES
  LodePNG_debug_LIBRARIES
  LodePNG_util_debug_LIBRARIES
  LodePNG_INCLUDE_DIR
  LodePNG_LIBRARIES
  )
