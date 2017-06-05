# Find the Eigen Library
if(EIGEN_ROOT)
  message(STATUS "Hint EIGEN_ROOT as ${EIGEN_ROOT}")
endif()

find_path(EIGEN_INCLUDE_DIR Eigen/Eigen
	HINTS
  ${EIGEN_ROOT}
  ${PROJECT_SOURCE_DIR}/external/eigen
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EIGEN REQUIRED_VARS EIGEN_INCLUDE_DIR)

mark_as_advanced(EIGEN_INCLUDE_DIR EIGEN_ROOT)
