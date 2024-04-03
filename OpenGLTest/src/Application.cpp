#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>

#include "GLHandleError.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

struct Color
{
    float R;
    float G;
    float B;
    float A;
};

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

	int windowWidth = 1200;
	int windowHeight = 900;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Test", NULL, NULL);
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
		float squareSize = 400.0f;
		float padding = 0.0f;

		float verticesData[] = {
			padding, windowHeight - padding - squareSize, 0.0f, 0.0f,
			padding + squareSize, windowHeight - padding - squareSize, 1.0f, 0.0f,
			padding + squareSize, windowHeight - padding, 1.0f, 1.0f,
			padding, windowHeight - padding, 0.0f, 1.0f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		/* Enable blending and define a blend function */
		GL_CALL(glEnable(GL_BLEND));
		GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		/* Create a new VAO (Vertex Array Object) */
		VertexArray va;
        
		/* Create a new vertex buffer */
		VertexBuffer vb(verticesData, 4 * 4 * sizeof(float));

        /* Create vertex buffer layout */
        VertexBufferLayout layout;
		layout.Push(GL_FLOAT, 2);
		layout.Push(GL_FLOAT, 2);

        /* Add vertex buffer to VAO */
        va.AddBuffer(vb, layout);
        
        /* Create a new IBO (Index Buffer Object) */
		IndexBuffer ib(indices, 6);

		/* MVP matrices */
		glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f); // Maps what the "camera" sees to NDC (Normalized device coordinate), taking care of aspect ratio and perspective
		glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // Defines position and orientation of the "camera"

		/* Create the shader */
		//Shader shader("res/shaders/Basic.shader");
		Shader shader("res/shaders/BasicWithTexture.shader");
		shader.Bind();

		/* Create a texture */
		//Texture texture("res/textures/cat.png");
		Texture texture("res/textures/opengl-logo.png");

		/* Bind it and set a 1-integer uniform to the shader for the texture */
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		/* Unbind everything */
		va.Unbind();
        shader.Unbind();
        vb.Unbind();
		ib.Unbind();

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

		/*float R = 0.0f;
		float increment = 0.02f;
		Color uniformColor = { 0.0f, 0.584f, 0.141f, 1.0f };*/

		glm::vec3 translation(0, 0, 0);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			/* Re-bind shader */
			shader.Bind();

			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translation); // Defines position, rotation and scale of the vertices of the model in the world
			glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
			shader.SetUniformMat4f("u_MVP", mvp);

			/* Set color with a uniform */
			/*shader.SetUniform4f(
				"u_Color",
				R,
				uniformColor.G,
				uniformColor.B,
				uniformColor.A
			);*/

			/* Draw */
			renderer.Draw(va, ib, shader);

			/* Color animation */
			/*if (R >= 1.0f)
				increment = -0.02f;
			else if (R <= 0.0f)
				increment = 0.02f;

			R += increment;*/

			/* Setup ImGui window */
			{
				ImGui::Begin("Debug");

				//ImGui::Text("This is some useful text.");
				//ImGui::SliderFloat3("Translation", &translation.x, 0.0f, (float)windowWidth);

				ImGui::SliderFloat("Translation X", &translation.x, -(float)windowWidth, (float)windowWidth);
				ImGui::SliderFloat("Translation Y", &translation.y, -(float)windowHeight, (float)windowHeight);

				if (ImGui::Button("Reset Translation"))
					translation = glm::vec3(0, 0, 0);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
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
	}

	/* ImGui Cleanup */
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
