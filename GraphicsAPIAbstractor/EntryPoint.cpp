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

///Excuse the mess! :)

//A shader is a program that runs on the GPU. We do this because we want to be able to tell the GPU what to do. It is extremely powerful.
//All forms of information such as vertex position, color and textures originally given by us on the CPU are packed into a shader and sent to the GPU to be rendered. 
//We go from the vertex specification call to the vertex shader, to the fragment shader to the draw call and eventually a triangle is seen.

//A Vertex Shader is ran once per vertex. This tells OpenGL where we want the vertex to be on the screen space. It also passes data from attributes into the next stage. In our case, we have position.
//A fragment shader is run once for each pixel that needs to be rasterized. It decides what color each pixel is supposed to be. Thus, when a triangle has each vertex position decided, the fragment shader fills in the gap between the 3 positions for the triangle.
//If there are certain things you can do, try doing it in the Vertex Shader instead of a fragment shader. That is because running something 3 times per vertex vs 3000 times per pixel has a huge difference.
//Note that like everything else in OpenGL, we have to enable a shader to use it. 

//Vertex Buffers allow us to submit vertex data to the GPU via attributes.
//Uniforms also allow us to pass data from the CPU (C++ in this case) into the Shader so we can use it like a variable.
//Uniforms are set per draw before the actual drawing, while attributes are set via vertex.

//To tie together a buffer with a layout.  
//Vertex Arrays are OpenGL original, and are a way to bind Vertex Buffers with a certain specification - a layout.
//Instead of manually explictly specifying each Vertex Buffer's layout every time we rebind, we can make things more easier. Refer to this:
//Thus, the way we do things now change from Binding our Shader -> Binding our Vertex Buffer -> Setup the Vertex Layout -> Bind our Index Buffer -> Draw Call.
//To: Bind Shader -> Bind Vertex Array -> Bind Index Buffer -> Draw Call.
//Thus, Binding the Vertex Buffer and setting up its layout now becomes just binding the vertex layout.
//Technically speaking, Vertex Array objects are mandatory and are always being used. 
//The compatability OpenGL profile makes Vertex Array Object 0 a default object. The core OpenGL profile makes VAO object 0 not an object at all. 
//You can either have 1 VAO for your entire program lifespan and bind a Vertex Buffer and give it a specification each time you use it to draw something, or create a unique VAO for each unique piece of Geometry. Which is better? It depends.

//Index/Element Buffers allow us to reuse existing vertices. Thus, you can use already drawn vertices and copy + put them at another position. 
//This allows us to avoid having the same memory type in your GPU multiple times. 
//Each vertice may have so many properties in 3D models that re-rendering each one without index buffering can become really crazy and expensive. 
//You should be using index buffers for pretty much everything you do. :)

//Materials are essentially a Shader + a set of data (uniforms). A color would be a per object uniform that is stored in a material with its shader. 
//Thus, what a renderer would do is bind the shader in the material and setup all the uniforms within to draw it. 

struct FluctuatingColors //Temporary
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

    std::cout << glGetString(GL_VERSION) << "\n";
    std::cout << glGetString(GL_VENDOR) << "\n";
    std::cout << glGetString(GL_RENDERER) << "\n";
    //Vertexes are points on a piece of geometry. A triangle has 3 points, for example, and lines are drawn between them and filled in to create said triangle.
    {
        float positions[] =
        {
            -0.5f, -0.5,  //0
            0.5f, -0.5f,  //1
            0.5f, 0.5f,   //2
            -0.5f, 0.5f,  //3
        };

        unsigned int indices[] =     //Index Buffer
        {
            0, 1, 2,
            2, 3, 0
        };


        VertexArray vertexArray;
        VertexBuffer vertexBuffer(positions, 4 * 2 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        vertexArray.AddBuffer(vertexBuffer, layout);

        IndexBuffer indexBuffer(indices, 6);
        Shader shader("OpenGL/Shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

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

