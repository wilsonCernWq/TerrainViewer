#--- Project Titles
get_filename_component(PROJECTNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
cmake_minimum_required(VERSION 2.8)
project(${PROJECTNAME})

#--- Libraries Setups
include(common/icg_settings.cmake)

#--- Subdirectories
set(projdir ${CMAKE_CURRENT_LIST_DIR}/program)
file(GLOB children RELATIVE ${projdir} ${projdir}/*)
foreach(child ${children})
  if(IS_DIRECTORY ${projdir}/${child})
      add_subdirectory(${projdir}/${child})
  endif()
endforeach()
