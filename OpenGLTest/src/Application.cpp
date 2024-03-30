#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

/* Shaders */

enum class ShaderType
{
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
};

struct Color
{
    float R;
    float G;
    float B;
    float A;
};

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                // Setting the mode/type to vertex
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                // Setting the mode/type to fragment
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        
        //char message[length];
        char* message = (char*)alloca(sizeof(char) * length);
        glGetShaderInfoLog(id, length, &length, message);
        
        Log("Failed to compile " + std::string(type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader!");
        Log(message);
        
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    /*glDetachShader(vs);
    glDetachShader(fs);*/

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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
		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		GL_CALL(glUseProgram(shader));

		GL_CALL(int uniformLocation = glGetUniformLocation(shader, "u_Color"));
		ASSERT(uniformLocation != -1);

		/* Unbind everything */
		GL_CALL(glBindVertexArray(0));
		GL_CALL(glUseProgram(0));
		vb.Unbind();
		ib.Unbind();

		float R = 0.0f;
		float increment = 0.02f;
		Color uniformColor = { 0.0f, 0.584f, 0.141f, 1.0f };

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

			/* Re-use program */
			GL_CALL(glUseProgram(shader));

			/* Set color with a uniform */
			GL_CALL(glUniform4f(
				uniformLocation,
				R,
				uniformColor.G,
				uniformColor.B,
				uniformColor.A
			));

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

		glDeleteProgram(shader);
	}

    glfwTerminate();
    return 0;
}
