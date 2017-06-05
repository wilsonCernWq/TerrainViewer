#pragma once
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "configure.h"

static enum { KEY_EMPTY, KEY_Z } modify_key;

namespace icg {

	/**
	 * @brief UpdateTransform Compute the projection matrix
	 */
	void UpdateMatrices(bool update_camera = true, bool update_view = true, bool update_projection = true, bool update_objects = true) {
		if (update_camera) {
			model::camera->UpdateCamera();
			model::light.GetCamera().UpdateCamera();
		}
		if (update_view) {
			model::camera->UpdateView();
			model::light.GetCamera().UpdateView();
		}
		if (update_projection) {
			GLfloat aspect = (GLfloat)sysinfo::WIDTH / (GLfloat)sysinfo::HEIGHT;
			model::camera->UpdateProjection(aspect, 0.5f, 0.5f, 1.0f, 500.0f);
			model::light.GetCamera().UpdateProjection(aspect, 50.0f, 50.0f, -50.0f, 100.0f);
		}
		if (update_objects) {
			model::light.UpdateMatrix();
			model::terrain.UpdateMatrix();
		}
	}

	/**
	 * @brief GetMouseButton: Mouse button handling function
	 * @param button (GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON)
	 * @param state  (GLUT_UP or GLUT_DOWN)
	 * @param x
	 * @param y
	 */
	void GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
		ImGui_ImplGlut_MouseCallback(button, state, x, y);
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse) { return; }
		static cy::Point2f p;
		helper::mouse2screen(x, y, sysinfo::WIDTH, sysinfo::HEIGHT, p);
		int modkey = glutGetModifiers();
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			if (modkey == GLUT_ACTIVE_CTRL || (modify_key == KEY_Z)) {
				model::light.GetBall()->BeginDrag(p[0], p[1]);
			}
			else {
				model::camera->GetBall()->BeginDrag(p[0], p[1]);
			}
			sysinfo::RotateMode = sysinfo::ROTATE;
		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			if (modkey == GLUT_ACTIVE_CTRL || (modify_key == KEY_Z)) {
				model::light.GetBall()->BeginZoom(p[0], p[1]);
			}
			else {
				model::camera->GetBall()->BeginZoom(p[0], p[1]);
			}
			sysinfo::RotateMode = sysinfo::ZOOM;
		}
	}

	/**
	 * @brief GetMouseMotion: Mouse position callback handling function
	 * @param x
	 * @param y
	 */
	void GetMouseMotion(GLint x, GLint y) {
		ImGui_ImplGlut_MotionCallback(x, y);
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse) { return; }
		static cy::Point2f p;
		helper::mouse2screen(x, y, sysinfo::WIDTH, sysinfo::HEIGHT, p);
		int modkey = glutGetModifiers();
		switch (sysinfo::RotateMode) {
		case (sysinfo::ROTATE):
			if (modkey == GLUT_ACTIVE_CTRL || (modify_key == KEY_Z)) {
				model::light.GetBall()->Drag(p[0], p[1]);
			}
			else {
				model::camera->GetBall()->Drag(p[0], p[1]);
			}
			break;
		case (sysinfo::ZOOM):
			if (modkey == GLUT_ACTIVE_CTRL || (modify_key == KEY_Z)) {
				model::light.GetBall()->Zoom(p[0], p[1]);
			}
			else {
				model::camera->GetBall()->Zoom(p[0], p[1]);
			}
			break;
		}
		UpdateMatrices();
	}

	void GetMousePassiveMotion(int x, int y) {
		ImGui_ImplGlut_PassiveMotionCallback(x, y);
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse) { return; }
	}

	void GetMouseWheel(int button, int dir, int x, int y) {
		ImGui_ImplGlut_MouseWheelCallback(button, dir, x, y);
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse) { return; }
	}

	void ResizeCallback(int w, int h) {
		sysinfo::WIDTH = static_cast<unsigned int>(w);
		sysinfo::HEIGHT = static_cast<unsigned int>(h);
		glViewport(0, 0, w, h);
		UpdateMatrices();
	}

	void GetNormalKey(unsigned char key, GLint x, GLint y) {
		static int screenshotcount = 0;
		std::vector<unsigned char> *framebuffer;
		switch (key) {
		case (27):  // Exit the program
			exit(EXIT_SUCCESS);
			break;
		case ((int)'S'):
		case ((int)'s'):
			framebuffer = new std::vector<unsigned char>(4 * sysinfo::WIDTH * sysinfo::HEIGHT);
			glReadPixels(0, 0, sysinfo::WIDTH, sysinfo::HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer->data());
			helper::saveimg("screenshot", ++screenshotcount, *framebuffer, sysinfo::WIDTH, sysinfo::HEIGHT);
			delete framebuffer;
			break;
		case ((int)'B'):
		case ((int)'b'):
			sysinfo::bgchange = !sysinfo::bgchange;
			sysinfo::bgcolor[0] = 0.0f;
			sysinfo::bgcolor[1] = 0.0f;
			sysinfo::bgcolor[2] = 0.0f;
			if (sysinfo::bgchange) {
				std::cout << "Start animating background" << std::endl;
			}
			else {
				std::cout << "Stop animating background" << std::endl;
			}
			break;
		case ((int)'P'):
		case ((int)'p'):
			std::cout << "window width " << sysinfo::WIDTH << " height " << sysinfo::HEIGHT << std::endl;
			std::cout << "camera" << std::endl;
			model::camera->PrintSelf(std::cout, 2);
			std::cout << "light" << std::endl;
			model::light.GetCamera().PrintSelf(std::cout, 2);
			break;
		case ((int)'Z'):
		case ((int)'z'):
			modify_key = KEY_Z;
			break;
		default:
			ImGui_ImplGlut_KeyboardCallback(key, x, y);
			break;
		}
	}

	void GetNormalKeyUp(unsigned char key, GLint x, GLint y) {
		switch (key) {
		case ((int)'Z'):
		case ((int)'z'):
			modify_key = KEY_EMPTY;
			break;
		default:
			break;
		}
	}

	void GetSpecialKey(int key, int x, int y) {
		static bool mode = true;
		switch (key) {
		case (GLUT_KEY_UP):
			model::terrain.MoveUp();
			break;
		case (GLUT_KEY_DOWN):
			model::terrain.MoveDown();
			break;
		case (GLUT_KEY_LEFT):
			model::terrain.MoveLeft();
			break;
		case (GLUT_KEY_RIGHT):
			model::terrain.MoveRight();
			break;
		case (GLUT_KEY_F6):
			std::cout << "Recompiling Shaders" << std::endl;
			model::light.CompileShaders();
			model::noise.CompileShaders();
			model::terrain.CompileShaders();
			std::cout << "Compilation Done" << std::endl;
			break;
		case (GLUT_KEY_F3):
			if (mode) {
				std::cout << "Wireframe Mode" << std::endl;
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				std::cout << "Surface Mode" << std::endl;
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			mode = !mode;
			break;
		case (GLUT_KEY_F2):
			std::cout << "Reset all rotation matrices" << std::endl;
			model::camera->GetBall()->Reset();
			model::light.GetBall()->Reset();
			UpdateMatrices();
			break;
		case (GLUT_KEY_F1):
			std::cout << "Switch to debug mode" << std::endl;
			GLDEBUG = true;
			break;
		default:
			ImGui_ImplGlut_KeyboardSpecialCallback(key, x, y);
			break;
		}
	}

	void GetSpecialKeyUp(int key, int x, int y) {
		switch (key) {
		case (GLUT_KEY_F1):
			std::cout << "Switch to normal mode" << std::endl;
			GLDEBUG = false;
			break;
		default:
			break;
		}
	}

	void GetIdleAction() {
		static float idlecount = 0.0f;
		if (sysinfo::bgchange) {
			idlecount += 0.01;
			sysinfo::bgcolor[0] = 0.1f * static_cast<float>(sin(idlecount));
			sysinfo::bgcolor[1] = 0.1f * static_cast<float>(sin(1.0f / 3.0f * cy::cyPi<float>() + idlecount));
			sysinfo::bgcolor[2] = 0.1f * static_cast<float>(sin(2.0f / 3.0f * cy::cyPi<float>() + idlecount));
		}
		glutPostRedisplay();
	}

};

#endif//_CALLBACKS_H_
