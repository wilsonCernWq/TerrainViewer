// ImGui GLUT binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#pragma once
#ifndef _IMGUI_IMPL_GLUT_H_
#define _IMGUI_IMPL_GLUT_H_

// struct GLFWwindow;
IMGUI_API bool        ImGui_ImplGlut_Init(bool install_callbacks);
IMGUI_API void        ImGui_ImplGlut_Shutdown();
IMGUI_API void        ImGui_ImplGlut_NewFrame();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplGlut_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplGlut_CreateDeviceObjects();

// GLUT callbacks (installed by default if you enable 'install_callbacks' during initialization)
// Provided here if you want to chain callbacks.
// You can also handle inputs yourself and use those as a reference.
/* IMGUI_API void        ImGui_ImplGlut_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods); */
/* IMGUI_API void        ImGui_ImplGlut_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset); */
/* IMGUI_API void        ImGui_ImplGlut_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods); */
/* IMGUI_API void        ImGui_ImplGlut_CharCallback(GLFWwindow* window, unsigned int c); */
IMGUI_API void ImGui_ImplGlut_KeyboardSpecialCallback(int key, int x, int y);
IMGUI_API void ImGui_ImplGlut_KeyboardCallback(unsigned char nChar, int x, int y);
IMGUI_API void ImGui_ImplGlut_MouseCallback(int button, int state, int x, int y);
IMGUI_API void ImGui_ImplGlut_MouseWheelCallback(int button, int dir, int x, int y);
IMGUI_API void ImGui_ImplGlut_MotionCallback(int x, int y);
IMGUI_API void ImGui_ImplGlut_PassiveMotionCallback(int x, int y);

#endif//_IMGUI_IMPL_GLUT_H_
