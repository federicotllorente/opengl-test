#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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
		float positions[] = {
			-0.5, -0.5,
			0.5, -0.5,
			0.5, 0.5,
			-0.5, 0.5
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		/* Create a new VAO (Vertex Array Object) */
		VertexArray va;
        
		/* Create a new vertex buffer */
		VertexBuffer vb(positions, 8 * sizeof(float));

        /* Create vertex buffer layout */
        VertexBufferLayout layout;
        layout.Push(GL_FLOAT, 2);

        /* Add vertex buffer to VAO */
        va.AddBuffer(vb, layout);
        
        /* Create a new IBO (Index Buffer Object) */
		IndexBuffer ib(indices, 6);

		/* Create the shader */
		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		/* Unbind everything */
		va.Unbind();
        shader.Unbind();
        vb.Unbind();
		ib.Unbind();

		float R = 0.0f;
		float increment = 0.02f;
		Color uniformColor = { 0.0f, 0.584f, 0.141f, 1.0f };

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

			/* Re-bind shader */
			shader.Bind();

			/* Set color with a uniform */
			shader.SetUniform4f(
				"u_Color",
				R,
				uniformColor.G,
				uniformColor.B,
				uniformColor.A
			);

			/* Re-bind VAO */
			va.Bind();

			/* Re-bind index buffer */
			ib.Bind();

            /* Draw */
			GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            /* Color animation */
			if (R >= 1.0f)
				increment = -0.02f;
			else if (R <= 0.0f)
				increment = 0.02f;

			R += increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

    glfwTerminate();
    return 0;
}
