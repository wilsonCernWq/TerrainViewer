#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "configure.h"

void RenderGUI()
{
	ImGui_ImplGlut_NewFrame();
	{
		ImGui::Text("Terrain Parameter");
		ImGui::SliderFloat("octave", &(model::terrain.octave), 1.0f, 10.0f);
		ImGui::SliderFloat("persistence", &(model::terrain.persistence), 0.0f, 2.0f);
		ImGui::SliderFloat("lacunarity", &(model::terrain.lacunarity), 0.0f, 5.0f);
		ImGui::SliderFloat("gain", &(model::terrain.gain), 0.0f, 10.0f);
		ImGui::SliderFloat("offset", &(model::terrain.offset), 0.0f, 1.0f);
		ImGui::SliderFloat("sand level", &(model::terrain.sandlevel), 0.0f, 0.3f);
		ImGui::SliderFloat("snow level", &(model::terrain.snowlevel), 0.0f, 0.3f);
		ImGui::SliderFloat("max height", &(model::terrain.maxlevel), 0.0f, 0.6f);
		ImGui::Text("Control");
		ImGui::SliderFloat("move speed", &(model::terrain.movespeed), 0.0f, 0.02f);
	}
	ImGui::Render();
}

void Render()
{
	//!
	//! set title
	sysinfo::fps.Count();
	sysinfo::title.str("");
	sysinfo::title << "terrain viewer" << " fps = " << sysinfo::fps.FPS();
	glutSetWindowTitle(sysinfo::title.str().c_str());
	//!
	//!
	model::light.BindShadow();
	icg::UpdateMatrices();
	model::terrain.Draw(true);
	model::light.UnbindShadow();
	icg::UpdateMatrices();
	//!
	//!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model::light.Draw();
	model::terrain.Draw(false);
	//!
	//! debug options
	// model::noise.DrawDisplay();
	// model::light.DrawDebugQuad();
	//!
	//!
	RenderGUI();
	glutSwapBuffers();
}

void PreRender()
{
	model::noise.Draw();
}

int main(int argc, char **argv)
{
	// initialization GLUT
	{
		// initialization GLUT
		{
			glutInit(&argc, argv);
			glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
			glutInitWindowPosition(sysinfo::WINX, sysinfo::WINY);
			glutInitWindowSize(sysinfo::WIDTH, sysinfo::HEIGHT);
			glutCreateWindow(argv[0]);
		}

		// Initialize GLEW
		std::cout << "-----------------------------------" << std::endl;
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cerr << "Error: Cannot Initialize GLEW "
				<< glewGetErrorString(err)
				<< std::endl;
			return EXIT_FAILURE;
		}
		else {
			std::cout << "Message: Using GLEW "
				<< glewGetString(GLEW_VERSION)
				<< std::endl;
		};

		std::cout << std::endl;
		std::cout << "Press ESC to exit" << std::endl;
		std::cout << "Use left mouse button to adjust object rotation" << std::endl;
		std::cout << "Use right mouse button to adjust camera distance" << std::endl;
		std::cout << "Use F1 to see debug mode" << std::endl;
		std::cout << "Use F2 to reset camera" << std::endl;
		std::cout << "Use F3 to see framewire" << std::endl;
		std::cout << "Use F6 to see recompile shader" << std::endl;
		std::cout << "Use CTRL/Z with left mouse key to adjust light position" << std::endl;
		std::cout << "Use arrow keys to move on terrain" << std::endl;
		std::cout << "Use key P to print camera/light information" << std::endl;
		std::cout << "Use key S to save screen shot" << std::endl;

		if (!model::noise.Init(2048, 2048)) { exit(EXIT_FAILURE); }

		model::light.RegisterCamera(model::camera);
		if (!model::light.Init()) { exit(EXIT_FAILURE); }

		model::terrain.RegisterCamera(model::camera);
		model::terrain.RegisterLight(model::light);
		model::terrain.RegisterNoise(model::noise);
		if (!model::terrain.Init()) { exit(EXIT_FAILURE); }

		icg::UpdateMatrices();
		check_error_gl("end of initialization in main");
	}

	// execute the program
	{
		// debug functions
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		// display
		glutDisplayFunc(Render);
		glutIdleFunc(icg::GetIdleAction);
		glutReshapeFunc(icg::ResizeCallback);
		// mouse
		glutMouseFunc(icg::GetMouseButton);
		glutMouseWheelFunc(icg::GetMouseWheel);
		glutMotionFunc(icg::GetMouseMotion);
		glutPassiveMotionFunc(icg::GetMousePassiveMotion);
		// keyboard
		glutKeyboardFunc(icg::GetNormalKey);
		glutKeyboardUpFunc(icg::GetNormalKeyUp);
		glutSpecialFunc(icg::GetSpecialKey);
		glutSpecialUpFunc(icg::GetSpecialKeyUp);
		// enter the main loop
		glutInitContextFlags(GLUT_DEBUG);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(sysinfo::bgcolor[0], sysinfo::bgcolor[1], sysinfo::bgcolor[2], 1.0f);
		// start rendering
		PreRender();
		ImGui_ImplGlut_Init(false);
		glutMainLoop();
		ImGui_ImplGlut_Shutdown();
	}
	// exit
	return EXIT_SUCCESS;
}
