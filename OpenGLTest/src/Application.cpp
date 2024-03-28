#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define DEBUG

#ifdef DEBUG
    #define GL_CALL(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
    #define GL_CALL(x) x
#endif

/* Logging */

static void Log(std::string message)
{
    std::cout << "[Logger] " << message << std::endl;
}

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] Code " << error << ": " << function << " in " << file << ":" << line << std::endl;
        return false;
    }
    
    return true;
}

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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Check if GLEW has been initialized properly */
    if (glewInit() != GLEW_OK)
        return -1;

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

    /* Create and bind a new buffer */
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /* Provide data to buffer */
    glBufferData(GL_ARRAY_BUFFER, 10 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, // index
        2, // size
        GL_FLOAT, // data type
        GL_FALSE, // should it be normalized?
        sizeof(float) * 2, // stride - the amount of memory we need to go to the next vertex (for more complex data structures we should use e.g. structs)
        0 // pointer - the amount of memory we need to go to the next parameter (in this case, the next axis point)
    );

    /* Create an IBO (Index Buffer Object) */
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    /* Create the shader */
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    int uniformLocation = glGetUniformLocation(shader, "u_Color");
    ASSERT(uniformLocation != -1);
    
    Color uniformColor { 1.0, 0.584, 0.141, 1.0 };
    glUniform4f(
        uniformLocation,
        uniformColor.R,
        uniformColor.G,
        uniformColor.B,
        uniformColor.A
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_POLYGON, 0, 5);
        GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
