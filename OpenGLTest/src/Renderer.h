#pragma once

#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define DEBUG

#ifdef DEBUG
    #define GL_CALL(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
    #define GL_CALL(x) x
#endif

void Log(std::string message);
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
