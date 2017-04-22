# Find the OpenGL Mathematics library (GLM)
if(GLM_ROOT)
  message(STATUS "Hint GLM_ROOT as ${GLM_ROOT}")
endif()

find_path(GLM_INCLUDE_DIR glm/glm.hpp
	HINTS
  ${GLM_ROOT}
	$ENV{GLM}
  ${PROJECT_SOURCE_DIR}/external/glm
	PATH_SUFFIXES glm
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM REQUIRED_VARS GLM_INCLUDE_DIR)

mark_as_advanced(GLM_INCLUDE_DIR GLM_ROOT)
