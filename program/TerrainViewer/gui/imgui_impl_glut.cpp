// ImGui GLUT binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <imgui.h>
#include "imgui_impl_glut.h"

// // GL3W/GLFW
// #include <GL/gl3w.h>
// #include <GLFW/glfw3.h>
// #ifdef _WIN32
// #undef APIENTRY
// #define GLFW_EXPOSE_NATIVE_WIN32
// #define GLFW_EXPOSE_NATIVE_WGL
// #include <GLFW/glfw3native.h>
// #endif

#include <GL/glew.h>
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__) // windows need special header
#    include <Windows.h>
#  else
#    if __unix__ // linux needs extensions for framebuffers
#      define GL_GLEXT_PROTOTYPES 1
#      include <GL/glext.h>
#    endif
#  endif
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#  include <GL/freeglut.h>
#endif

#include <iostream>

bool KeyboardEvent_(unsigned char nChar, int nX, int nY)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(nChar);

	if (nChar == 27) //Esc-key
		glutLeaveMainLoop();

	return true;
}

bool MouseEvent_(int button, int state, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);

	if (state == GLUT_DOWN && (button == GLUT_LEFT_BUTTON))
		io.MouseDown[0] = true;
	else
		io.MouseDown[0] = false;

	if (state == GLUT_DOWN && (button == GLUT_RIGHT_BUTTON))
		io.MouseDown[1] = true;
	else
		io.MouseDown[1] = false;

	if (state == GLUT_DOWN && (button == GLUT_MIDDLE_BUTTON))
		io.MouseDown[2] = true;
	else
		io.MouseDown[2] = false;

	return true;
}

// ==> glutSpecialFunc
void ImGui_ImplGlut_KeyboardSpecialCallback(int key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(key);
}

// ==> glutKeyboardFunc
void ImGui_ImplGlut_KeyboardCallback(unsigned char nChar, int x, int y)
{
	if (KeyboardEvent_(nChar, x, y))
	{
		glutPostRedisplay();
	}
}

// glutMouseWheelFunc
void ImGui_ImplGlut_MouseWheelCallback(int button, int dir, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	if (dir > 0)
	{
		// Zoom in
		io.MouseWheel = 1.0f;
	}
	else if (dir < 0)
	{
		// Zoom out
		io.MouseWheel = -1.0f;
	}
}

// ==> glutMouseFunc
// When a user presses and releases mouse buttons in the window, each press and each release generates a mouse callback.
// button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON
// state : GLUT_UP or GLUT_DOWN
// The x and y callback parameters indicate the window relative coordinates when the mouse button state changed
void ImGui_ImplGlut_MouseCallback(int button, int state, int x, int y)
{
	if (MouseEvent_(button, state, x, y))
	{
		glutPostRedisplay();
	}
}

// ==> glutMotionFunc
// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed. 
void ImGui_ImplGlut_MotionCallback(int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	glutPostRedisplay();
}

// ==> glutPassiveMotionFunc
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void ImGui_ImplGlut_PassiveMotionCallback(int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	glutPostRedisplay();
}

// Data
// static GLFWwindow*  g_Window = NULL;
// static bool         g_MousePressed[3] = { false, false, false };
// static float        g_MouseWheel = 0.0f;
static double       g_Time = 0.0f;
static GLuint       g_FontTexture = 0;
static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplGlut_RenderDrawLists(ImDrawData* draw_data)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	const float ortho_projection[4][4] =
	{
	  { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
	  { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
	  { 0.0f,                  0.0f,                  -1.0f, 0.0f },
	  {-1.0f,                  1.0f,                   0.0f, 1.0f },
	};
	glUseProgram(g_ShaderHandle);
	glUniform1i(g_AttribLocationTex, 0);
	glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
	glBindVertexArray(g_VaoHandle);

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Restore modified GL state
	glUseProgram(last_program);
	glActiveTexture(last_active_texture);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFunc(last_blend_src, last_blend_dst);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

static const char* ImGui_ImplGlut_GetClipboardText(void*)
{
	std::cerr << "ERROR GetClipboardText is not available for glut" << std::endl;
	return NULL; // glfwGetClipboardString(g_Window);
}

static void ImGui_ImplGlut_SetClipboardText(void*, const char* text)
{
	std::cerr << "ERROR SetClipboardText is not available for glut" << std::endl;
	// glfwSetClipboardString(g_Window, text);
}

// void ImGui_ImplGlut_MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/)
// {
//     if (action == GLFW_PRESS && button >= 0 && button < 3)
//         g_MousePressed[button] = true;
// }

// void ImGui_ImplGlut_ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset)
// {
//     g_MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
// }

// void ImGui_ImplGlut_KeyCallback(GLFWwindow*, int key, int, int action, int mods)
// {
//     ImGuiIO& io = ImGui::GetIO();
//     if (action == GLFW_PRESS)
//         io.KeysDown[key] = true;
//     if (action == GLFW_RELEASE)
//         io.KeysDown[key] = false;

//     (void)mods; // Modifiers are not reliable across systems
//     io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
//     io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
//     io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
//     io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
// }

// void ImGui_ImplGlut_CharCallback(GLFWwindow*, unsigned int c)
// {
//     ImGuiIO& io = ImGui::GetIO();
//     if (c > 0 && c < 0x10000)
//         io.AddInputCharacter((unsigned short)c);
// }

bool ImGui_ImplGlut_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	// Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);

	return true;
}

bool ImGui_ImplGlut_CreateDeviceObjects()
{
	// Backup GL state
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar *vertex_shader =
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"#version 330\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	g_ShaderHandle = glCreateProgram();
	g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
	g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
	glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
	glCompileShader(g_VertHandle);
	glCompileShader(g_FragHandle);
	glAttachShader(g_ShaderHandle, g_VertHandle);
	glAttachShader(g_ShaderHandle, g_FragHandle);
	glLinkProgram(g_ShaderHandle);

	g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
	g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
	g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
	g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
	g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

	glGenBuffers(1, &g_VboHandle);
	glGenBuffers(1, &g_ElementsHandle);

	glGenVertexArrays(1, &g_VaoHandle);
	glBindVertexArray(g_VaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
	glEnableVertexAttribArray(g_AttribLocationPosition);
	glEnableVertexAttribArray(g_AttribLocationUV);
	glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

	ImGui_ImplGlut_CreateFontsTexture();

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);

	return true;
}

void    ImGui_ImplGlut_InvalidateDeviceObjects()
{
	if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
	if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
	if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
	g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

	glDetachShader(g_ShaderHandle, g_VertHandle);
	glDeleteShader(g_VertHandle);
	g_VertHandle = 0;

	glDetachShader(g_ShaderHandle, g_FragHandle);
	glDeleteShader(g_FragHandle);
	g_FragHandle = 0;

	glDeleteProgram(g_ShaderHandle);
	g_ShaderHandle = 0;

	if (g_FontTexture)
	{
		glDeleteTextures(1, &g_FontTexture);
		ImGui::GetIO().Fonts->TexID = 0;
		g_FontTexture = 0;
	}
}

bool    ImGui_ImplGlut_Init(bool install_callbacks)
{
	// g_Window = window;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = 9;                     // tab
	io.KeyMap[ImGuiKey_LeftArrow] = GLUT_KEY_LEFT;         // Left
	io.KeyMap[ImGuiKey_RightArrow] = GLUT_KEY_RIGHT;        // Right
	io.KeyMap[ImGuiKey_UpArrow] = GLUT_KEY_UP;           // Up
	io.KeyMap[ImGuiKey_DownArrow] = GLUT_KEY_DOWN;         // Down
	io.KeyMap[ImGuiKey_PageUp] = GLUT_KEY_PAGE_UP;      // PageUp
	io.KeyMap[ImGuiKey_PageDown] = GLUT_KEY_PAGE_DOWN;    // PageDown
	io.KeyMap[ImGuiKey_Home] = GLUT_KEY_HOME;         // Home
	io.KeyMap[ImGuiKey_End] = GLUT_KEY_END;          // End
	io.KeyMap[ImGuiKey_Delete] = 127;                   // Delete
	io.KeyMap[ImGuiKey_Backspace] = 8;                     // Backspace
	io.KeyMap[ImGuiKey_Enter] = 13;                    // Enter
	io.KeyMap[ImGuiKey_Escape] = 27;                    // Escape
	io.KeyMap[11] = 1;                     // ctrl-A
	io.KeyMap[12] = 3;                     // ctrl-C
	io.KeyMap[13] = 22;                    // ctrl-V
	io.KeyMap[14] = 24;                    // ctrl-X
	io.KeyMap[15] = 25;                    // ctrl-Y
	io.KeyMap[16] = 26;                    // ctrl-Z
	// io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	// io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	// io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	// io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	// io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	// io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	// io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	// io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	// io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	// io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	// io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	// io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	// io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	// io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	// io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	// io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	// io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	// io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	// io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	// Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.RenderDrawListsFn = ImGui_ImplGlut_RenderDrawLists;
	io.SetClipboardTextFn = ImGui_ImplGlut_SetClipboardText;
	io.GetClipboardTextFn = ImGui_ImplGlut_GetClipboardText;
	io.MouseDrawCursor = false;
#ifdef _WIN32
	// io.ImeWindowHandle = glfwGetWin32Window(g_Window);
#endif

	if (install_callbacks)
	{
		// glfwSetMouseButtonCallback(window, ImGui_ImplGlut_MouseButtonCallback);
		// glfwSetScrollCallback(window, ImGui_ImplGlut_ScrollCallback);
		// glfwSetKeyCallback(window, ImGui_ImplGlut_KeyCallback);
		// glfwSetCharCallback(window, ImGui_ImplGlut_CharCallback);
	}

	return true;
}

void ImGui_ImplGlut_Shutdown()
{
	ImGui_ImplGlut_InvalidateDeviceObjects();
	ImGui::Shutdown();
}

void ImGui_ImplGlut_NewFrame()
{
	if (!g_FontTexture)
		ImGui_ImplGlut_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	w = glutGet(GLUT_WINDOW_WIDTH);
	h = glutGet(GLUT_WINDOW_HEIGHT);
	// glfwGetWindowSize(g_Window, &w, &h);
	GLint dims[4] = { 0 };
	glGetIntegerv(GL_VIEWPORT, dims);
	display_w = dims[2];
	display_h = dims[3];
	// glfwGetFramebufferSize(g_Window, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	float deltaTime = (float)(current_time - g_Time) / 1000.0f;
	io.DeltaTime = g_Time > 0.0 ? deltaTime : (float)(1.0f / 60.0f);
	g_Time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	// io.MousePos = ImVec2(-1,-1);
	// if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED))
	// {
	//     double mouse_x, mouse_y;
	//     glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
	//     io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
	// }
	// else
	// {
	//     io.MousePos = ImVec2(-1,-1);
	// }

	// for (int i = 0; i < 3; i++)
	//   {
	//     // // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	//     // io.MouseDown[i] = g_MousePressed[i] || glfwGetMouseButton(g_Window, i) != 0;
	//     // io.MouseDown[i] = false;
	//     // g_MousePressed[i] = false;
	//     // io.MouseDown[i] = false;
	//   }

	// io.MouseWheel = g_MouseWheel;
	// g_MouseWheel = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	// glfwSetInputMode(g_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

	// Start the frame
	ImGui::NewFrame();
}
