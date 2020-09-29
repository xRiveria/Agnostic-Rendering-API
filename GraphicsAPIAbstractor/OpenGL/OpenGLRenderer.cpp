#include "GAAPrecompiledHeader.h"
#include "OpenGLRenderer.h"
#include "GL/glew.h"

void GLClearError()  //Clears all errors.
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) //While we have errors here being retrieved...
    {
        std::cout << "[OpenGL Error!] (0x" << std::hex << error << std::dec << ") :" << "\nFunction: " << function << "\nFile: " << file << "\nLine: " << line << "\n";
        return false;
    }
    return true;
}

void OpenGLRenderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader)
{
    shader.Bind();
    vertexArray.Bind();
    indexBuffer.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr)); //We can put nullptr because the data is already bound to the buffer.
}

void OpenGLRenderer::PrintSystemInformation() const
{
    std::cout << glGetString(GL_VERSION) << "\n" << glGetString(GL_VENDOR) << "\n" << glGetString(GL_RENDERER) << "\n";
}



