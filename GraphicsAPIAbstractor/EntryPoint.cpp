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
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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

    //Vertexes are points on a piece of geometry. A triangle has 3 points, for example, and lines are drawn between them and filled in to create said triangle.
    {
        float positions[] =
        {
            100.0f, 100.0f, 0.0f, 0.0f,  //0
            200.0f, 100.0f, 1.0f, 0.0f, //1
            200.0f, 200.0f, 1.0f, 1.0f,  //2
            100.0f, 200.0f, 0.0f, 1.0f //3
        };

        float positionsAgain[] =
        {
            500.0f, 100.0f, 0.0f, 0.0f,  //0
            400.0f, 100.0f, 1.0f, 0.0f, //1
            400.0f, 200.0f, 1.0f, 1.0f,  //2
            500.0f, 200.0f, 0.0f, 1.0f //3
        };

        //Below we have our projection matrix. Anything bigger than what we specified for the bounds will not be rendered! 
        //Thus, ensure the positions above are within the bounds specified.
        //These positions below when multiplied with the above positions will be turned into that 1 to 1 normalized coordinate space.
        //At the end of the day, these vertex positions get multipled with our matrix and that is what converts them back to being -0.5 and 0.5 etc.
        
        glm::mat4 projectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -3.0f, 1.0f);

        glm::vec4 vp(100.0f, 100.0f, 0.0f, 1.0f);
        glm::vec4 result = projectionMatrix * vp;

        //We can see that we have successfully converted our vertex positions into that -1 to 1 space.
        //That is what projection does in both 2D and 3D, orthographic or perspective. All you're doing is telling your computer how to convert from whatever space you're dealing with (what you give it) to that -1 to 1 space. 

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
        renderer.PrintSystemInformation();

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


