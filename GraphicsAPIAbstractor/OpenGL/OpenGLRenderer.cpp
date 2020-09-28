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
