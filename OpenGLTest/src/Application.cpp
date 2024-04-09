#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>

#include "AppWindow.h"
#include "GLHandleError.h"

#include "tests/TestClearColor.h"
#include "tests/TestSquare.h"
#include "tests/TestSombrero.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//int WindowWidth = 1200;
//int WindowHeight = 900;
int WindowWidth = 900;
int WindowHeight = 900;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the GLFW library */
    if (!glfwInit())
        return -1;

    /* Set version to 3.3.x */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    /* Set profile to Core */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WindowWidth, WindowHeight, "OpenGL Test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    /* Check if GLEW has been initialized properly */
    if (glewInit() != GLEW_OK)
        return -1;

    std::cout << "[OpenGL Version] " << glGetString(GL_VERSION) << std::endl;
	
    /* Wrapping all this in a separate scope since OpenGL (`glGetError`) returns an error if there is no context */
    /* (Since `glfwTerminate` is being called at the end, which destroys the OpenGL context) */
    {
		/* Enable blending and define a blend function */
		GL_CALL(glEnable(GL_BLEND));
		GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		Renderer renderer;

		/* Create and setup ImGui context */
		const char* glsl_version = "#version 330 core";
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		IMGUI_CHECKVERSION();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
		io.FontGlobalScale = 1.7f; // Set global scale
		ImGui::StyleColorsDark(); // Set dark mode

		test::Test* currentTest = nullptr;
		test::TestMenu* menu = new test::TestMenu(currentTest);
		currentTest = menu;
		
		menu->RegisterTest<test::TestClearColor>("Clear color");
		menu->RegisterTest<test::TestSquare>("Square");
		menu->RegisterTest<test::TestSombrero>("Sombrero");

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (currentTest)
			{
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender(renderer);
				
				ImGui::Begin("Test");

				if (currentTest != menu)
				{
					if (ImGui::Button("<-"))
					{
						delete currentTest;
						currentTest = menu;
					}

					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				}

				currentTest->OnImGuiRender(io);	
				
				ImGui::End();
			}

			/* Render ImGui window */
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		/* Test Cleanup */
		delete currentTest;
		if (currentTest != menu)
			delete menu;
	}

	/* ImGui Cleanup */
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
