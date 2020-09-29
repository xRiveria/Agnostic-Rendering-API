#pragma once
#include "GL/glew.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#define ASSERT(x) if ((x == false)) __debugbreak();  //__ means Compiler Intrisic. This will only work in MSVS.
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();  //Clears all errors.
bool GLLogCall(const char* function, const char* file, int line);

class OpenGLRenderer
{
public:
    void Clear() const;
    void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader);
};



