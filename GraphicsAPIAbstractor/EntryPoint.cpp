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
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Tests/TestClearColor.h"

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
            -50.0f, -50.0f, 0.0f, 0.0f,  //0
             50.0f,-50.0f, 1.0f, 0.0f, //1
             50.0f, 50.0f, 1.0f, 1.0f,  //2
            -50.0f, 50.0f, 0.0f, 1.0f //3
        };

        //Below we have our projection matrix. Anything bigger than what we specified for the bounds will not be rendered! 
        //Thus, ensure the positions above are within the bounds specified.
        //These positions below when multiplied with the above positions will be turned into that 1 to 1 normalized coordinate space.
        //At the end of the day, these vertex positions get multipled with our matrix and that is what converts them back to being -0.5 and 0.5 etc.
        
        glm::mat4 projectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f); //Pixel based. Every unit is one pixel here.
        glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));  //Moves our camera to the right, so everything else moves left on the screen.

        //We can see that we have successfully converted our vertex positions into that -1 to 1 space.
        //That is what projection does in both 2D and 3D, orthographic or perspective. All you're doing is telling your computer how to convert from whatever space you're dealing with (what you give it) to that -1 to 1 space. 

        unsigned int indices[] =     //Index Buffer
        {
            0, 1, 2,
            2, 3, 0
        };
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //We're saying that for the source, take the source's Alpha, and when we try to render something on top of that, take 1 - the source Alpha = the destination alpha. 
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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


        Texture textureA("Resources/Textures/PrismEngineLogo.png");
        Texture textureB("Resources/Textures/AeternumGameLogo.png");
       // texture.Bind();
        //shader.SetUniform1i("u_Texture", 0); //0 because we bound our texture to slot 0 in Texture.cpp.
                                             //Texture Coordinates tell our geometry which part of the texture to sample from. Our Fragment/Pixel shader goes through and rasterizes the rectangle,  
                                             //The fragment shader is responsible for the color of each pixel. We need to somehow tell the fragment shader to sample from the texture pixels to decide which color the pixel on the geometry will be.
                                             //We are to specify for each vertex we have on our rectangle, what area of the texture it should be. The frag shader will turn interpolate between that so that if we're rendering a pixel halfway between 2indices, it will choose a coordinate that is halfway through as well.  
        vertexArray.Unbind();
        vertexBuffer.Unbind();
        indexBuffer.Unbind();
        shader.Unbind();
             
        OpenGLRenderer renderer;
        Test::TestClearColor test;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);
       /* bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45, 0.55f, 0.60f, 1.00f);*/

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();
            test.OnUpdate(0.0f);
            test.OnRender();

            //New Frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            test.OnImGuiRender();
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                //MVP - Model View Projection Matrix. Remember that this is in reverse because OpenGL's memory layout in its shader and GPU is column major, and that is why glm does this for us due to OpenGL.
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.Bind();
                textureA.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                shader.SetUniform1i("u_Texture", 0);
                renderer.Draw(vertexArray, indexBuffer, shader);
            }

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.Bind();
                textureB.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                shader.SetUniform1i("u_Texture", 0);
                renderer.Draw(vertexArray, indexBuffer, shader);
            }

            {
                ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
               
                ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);

                ImGui::Begin("Graphical Information");
                ImGui::Text(renderer.RetrieveGraphicalInformation().rendererInformation);
                ImGui::Text(renderer.RetrieveGraphicalInformation().vendorInformation);
                ImGui::Text(renderer.RetrieveGraphicalInformation().versionInformation);
                ImGui::End();

                /*static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world!");



                ImGui::Checkbox("Demo Window", &show_demo_window);
                ImGui::Checkbox("Another Window", &show_another_window);
                

                ImGui::ColorEdit3("clear color", (float*)&clear_color);
                if (ImGui::Button("Button"))
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);
                */
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            //Render
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;	
    //////////////////////
}


