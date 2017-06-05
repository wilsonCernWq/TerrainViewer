#
#--- to include IMGUI library
#
SET(ARCH "")
IF(WIN32)
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH /x64)
  ENDIF()
ENDIF()

SET(IMGUI_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/external/imgui)

IF (WIN32)
  FIND_LIBRARY(IMGUI_release_LIBRARIES NAMES imgui
    PATHS
    ${PROJECT_SOURCE_DIR}/external/imgui/lib/Release${ARCH}
    )
  FIND_LIBRARY(IMGUI_debug_LIBRARIES NAMES imgui
    PATHS
    ${PROJECT_SOURCE_DIR}/external/imgui/lib/Debug${ARCH}
    )
  SET(IMGUI_LIBRARIES
    optimized ${IMGUI_release_LIBRARIES}
    debug ${IMGUI_debug_LIBRARIES}
    )
ELSE()
  FIND_LIBRARY(IMGUI_LIBRARIES NAMES imgui
    PATHS
    ${PROJECT_SOURCE_DIR}/external/imgui/lib/unix
    )
ENDIF()

find_package_handle_standard_args(IMGUI
  DEFAULT_MSG
  IMGUI_INCLUDE_DIR
  IMGUI_LIBRARIES
  )

MARK_AS_ADVANCED(
  IMGUI_release_LIBRARIES
  IMGUI_debug_LIBRARIES
  IMGUI_INCLUDE_DIR
  IMGUI_LIBRARIES
  )
