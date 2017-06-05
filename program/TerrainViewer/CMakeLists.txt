#
# --- generate executable for each project
#
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
message(STATUS " * app ${EXERCISENAME}")
# search for files
# ---
file(GLOB_RECURSE CPP_GUI    "${CMAKE_CURRENT_LIST_DIR}/gui/*.cpp")
file(GLOB_RECURSE HPP_GUI    "${CMAKE_CURRENT_LIST_DIR}/gui/*.h")
# ---
file(GLOB_RECURSE SHADERS    "${CMAKE_CURRENT_LIST_DIR}/shaders/*.glsl")
file(GLOB_RECURSE ASSESTS    "${CMAKE_CURRENT_LIST_DIR}/assests/*")
file(GLOB_RECURSE SRC_BALL   "${CMAKE_CURRENT_LIST_DIR}/ball/*")
file(GLOB_RECURSE SRC_COMMON "${CMAKE_CURRENT_LIST_DIR}/common/*")
file(GLOB_RECURSE SRC_LOADER "${CMAKE_CURRENT_LIST_DIR}/loader/*")
file(GLOB_RECURSE SRC_OBJECT "${CMAKE_CURRENT_LIST_DIR}/object/*")
file(GLOB SRC_ROOT "*.h")
set(SRC ${SRC_COMMON} ${SRC_BALL} ${SRC_LOADER} ${SRC_OBJECT} ${SRC_ROOT} main.cpp)
source_group("common"   FILES ${SRC_COMMON})
source_group("ball"     FILES ${SRC_BALL}  )
source_group("loader"   FILES ${SRC_LOADER})
source_group("object"   FILES ${SRC_OBJECT})
source_group("shaders"  FILES ${SHADERS}   )
source_group("gui"      FILES ${CPP_GUI} ${HPP_GUI})
source_group(""         FILES ${SRC_ROOT} main.cpp)
# add libraries
IF(WIN32)
	add_library(imgui_freeglut ${CPP_GUI} ${HPP_GUI})
ELSE()
	add_library(imgui_freeglut SHARED ${CPP_GUI} ${HPP_GUI})
ENDIF()
target_link_libraries(imgui_freeglut ${COMMON_LIBS})
# add programs
add_executable(${EXERCISENAME} ${SRC} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS} imgui_freeglut)
# install dlls or shader
set(copyfiles ${COMMON_DLLS} ${SHADERS} ${ASSESTS})
message(STATUS " -- LIBS: ${COMMON_LIBS}")
message(STATUS " -- INSTALL: ${copyfiles}")
foreach(f ${copyfiles})
    if(EXECUTABLE_OUTPUT_PATH)
    file(COPY ${f} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
    else()
    file(COPY ${f} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endforeach()
