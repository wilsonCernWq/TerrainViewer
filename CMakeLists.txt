#
#--- Project Titles
GET_FILENAME_COMPONENT(PROJECTNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(${PROJECTNAME})
INCLUDE(cmake/common.cmake)
#
#--- Subdirectories
# SET(projdir ${CMAKE_CURRENT_LIST_DIR}/program)
# FILE(GLOB children RELATIVE ${projdir} ${projdir}/*)
# FOREACH(child ${children})
#   IF(IS_DIRECTORY ${projdir}/${child})
#       add_subdirectory(${projdir}/${child})
#   ENDIF()
# ENDFOREACH()
ADD_SUBDIRECTORY(program/TerrainViewer)
