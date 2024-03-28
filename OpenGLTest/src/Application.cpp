#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

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
    unsigned int vao;
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    /* Create a new vertex buffer */
    VertexBuffer vb(positions, 8 * sizeof(float));

    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(
        0, // index
        2, // size
        GL_FLOAT, // data type
        GL_FALSE, // should it be normalized?
        sizeof(float) * 2, // stride - the amount of memory we need to go to the next vertex (for more complex data structures we should use e.g. structs)
        0 // pointer - the amount of memory we need to go to the next parameter (in this case, the next axis point)
    ));

    /* Create a new IBO (Index Buffer Object) */
    IndexBuffer ibo(indices, 6);

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
    ibo.Unbind();
    //GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    //GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    
    float R = 0.0f;
    float increment = 0.02f;
    Color uniformColor = { 0.0f, 0.584f, 0.141f, 1.0f };

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
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

        //GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, shader));
        //GL_CALL(glEnableVertexAttribArray(0));
        //GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

        /* Re-bind VAO */
        GL_CALL(glBindVertexArray(vao));

        /* Re-bind IBO */
        ibo.Bind();
        //GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_POLYGON, 0, 5);
        GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

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
    glfwTerminate();
    return 0;
}
