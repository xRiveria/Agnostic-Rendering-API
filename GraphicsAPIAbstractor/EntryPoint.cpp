#include "GAAPrecompiledHeader.h"
#include "Renderer.h"
#include "OpenGL/OpenGLRenderer.h"
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "OpenGL/OpenGLRenderer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

//Math in graphics programming mainly involves Matrices and Vectors.
//A Matrix is basically an array of numbers which we can manipulate. Its useful for positions.
//There are 2 types of Vectors in programming - Directional Vectors and Positional Vectors. 
//A Vector in graphics programming can just be positions in 2D, 3D or 4D space.
//The most common usage for these is Transformations. 
//A transformation is used for example to position a ball in a 3D ball, or a camera positioning around the ball.
//There are no "cameras". What we are doing really is moving the camera and the ball around.
//We change the position of the camera and ball which creates the illusion of a camera orbiting around.
//We might also want to position vertices in a way that is just not a translation but also scale or rotation. All these things require Maths to accomplish.

//glm is a OpenGL specific Maths library and everything is laid out correctly for use.
//A Projection Matrix is a way for us to tell Windows how we want to map all our vertices to it.
//We have Vertex Buffers filled with vertex positions, but we need a way to transform them to a 2D plain.
//We need to tell all our vertex positions that the window we're drawing onto is a square. So do some Maths to make it work. 
//An orthographic matrix is a way for us to map our coordinates on a 2D plain whereby objects further away don't get smaller.
//Perspective projections are what we are used to in real life whereby objects furthur away are smaller. This is not needed for 2D rendering. 

struct FluctuatingColors
{
    float r = 0.0f;
    float increment = 0.05f;

    void SetColor(Shader& shader)
    {
        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
    }

    void TickColor()
    {
        if (r > 1.0f)
        {
            increment = -0.05f;
        }
        else if (r < 0.0f)
        {
            increment = 0.05f;
        }
        r += increment;
    }
};

int main()
{
    std::cout << "Start of Program!" << "\n";
    RendererAbstractor::Renderer::InitializeSelectedRenderer(RendererAbstractor::Renderer::API::OpenGL);

    ////////////////////// GLFW with OpenGL Context Creation
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }
    else
    {
        std::cout << glGetString(GL_VERSION) << "\n";
        std::cout << glGetString(GL_VENDOR) << "\n";
        std::cout << glGetString(GL_RENDERER) << "\n";
    }

    //Vertexes are points on a piece of geometry. A triangle has 3 points, for example, and lines are drawn between them and filled in to create said triangle.
    {
        float positions[] =
        {
            -0.5f, -0.5, 0.0f, 0.0f,  //0
            0.5f, -0.5f, 1.0f, 0.0f, //1
            0.5f, 0.5f, 1.0f, 1.0f,  //2
            -0.5f, 0.5f, 0.0f, 1.0f //3
        };

        unsigned int indices[] =     //Index Buffer
        {
            0, 1, 2,
            2, 3, 0
        };
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //We're saying that for the source, take the source's Alpha, and when we try to render something on top of that, take 1 - the source Alpha = the destination alpha. 
 
        VertexArray vertexArray;
        VertexBuffer vertexBuffer(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        vertexArray.AddBuffer(vertexBuffer, layout);

        IndexBuffer indexBuffer(indices, 6);
        glm::mat4 projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -3.0f, 1.0f);

        Shader shader("OpenGL/Shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
        shader.SetUniformMat4f("u_MVP", projectionMatrix);
        Texture texture("Resources/Textures/PrismEngineLogo.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0); //0 because we bound our texture to slot 0 in Texture.cpp.
                                             //Texture Coordinates tell our geometry which part of the texture to sample from. Our Fragment/Pixel shader goes through and rasterizes the rectangle,  
                                             //The fragment shader is responsible for the color of each pixel. We need to somehow tell the fragment shader to sample from the texture pixels to decide which color the pixel on the geometry will be.
                                             //We are to specify for each vertex we have on our rectangle, what area of the texture it should be. The frag shader will turn interpolate between that so that if we're rendering a pixel halfway between 2indices, it will choose a coordinate that is halfway through as well.  
        vertexArray.Unbind();
        vertexBuffer.Unbind();
        indexBuffer.Unbind();
        shader.Unbind();
        
        OpenGLRenderer renderer;
        FluctuatingColors fluctColor;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            fluctColor.SetColor(shader);
            renderer.Draw(vertexArray, indexBuffer, shader);
            fluctColor.TickColor();

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;	
    //////////////////////
}


