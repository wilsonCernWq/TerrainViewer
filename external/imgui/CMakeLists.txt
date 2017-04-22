IF (WIN32)
  add_library(imgui
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    )
ELSE()
  add_library(imgui SHARED
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    )
ENDIF()
