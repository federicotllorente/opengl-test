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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1200, 900, "OpenGL Test", NULL, NULL);
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
		float verticesData[] = {
			-0.5, -0.5, 0.0f, 0.0f,
			0.5, -0.5, 1.0f, 0.0f,
			0.5, 0.5, 1.0f, 1.0f,
			-0.5, 0.5, 0.0f, 1.0f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

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

		glm::mat4 projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

		/* Create the shader */
		//Shader shader("res/shaders/Basic.shader");
		Shader shader("res/shaders/BasicWithTexture.shader");
		shader.Bind();

		/* Create a texture */
		Texture texture("res/textures/cat.png");

		/* Bind it and set a 1-integer uniform to the shader for the texture */
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);
		shader.SetUniformMat4f("u_MVP", projectionMatrix);

		/* Unbind everything */
		va.Unbind();
        shader.Unbind();
        vb.Unbind();
		ib.Unbind();

		Renderer renderer;

		/*float R = 0.0f;
		float increment = 0.02f;
		Color uniformColor = { 0.0f, 0.584f, 0.141f, 1.0f };*/

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();

			/* Re-bind shader */
			shader.Bind();

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

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

    glfwTerminate();
    return 0;
}
