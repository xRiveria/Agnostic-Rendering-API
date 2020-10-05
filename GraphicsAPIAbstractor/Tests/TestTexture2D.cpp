#include "GAAPrecompiledHeader.h"
#include "TestTexture2D.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Test
{
    TestTexture2D::TestTexture2D() :
        m_ProjectionMatrix(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
        m_ViewMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
        m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0)
    {
        float positions[] =
        {
            -50.0f, -50.0f, 0.0f, 0.0f,  //0
             50.0f,-50.0f, 1.0f, 0.0f, //1
             50.0f, 50.0f, 1.0f, 1.0f,  //2
            -50.0f, 50.0f, 0.0f, 1.0f //3
        };

        unsigned int indices[] =     //Index Buffer
        {
            0, 1, 2,
            2, 3, 0,

        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //We're saying that for the source, take the source's Alpha, and when we try to render something on top of that, take 1 - the source Alpha = the destination alpha. 

        m_VertexArrayObject = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        m_VertexArrayObject->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
        //Below we have our projection matrix. Anything bigger than what we specified for the bounds will not be rendered! 
        //Thus, ensure the positions above are within the bounds specified.
        //These positions below when multiplied with the above positions will be turned into that 1 to 1 normalized coordinate space.
        //At the end of the day, these vertex positions get multipled with our matrix and that is what converts them back to being -0.5 and 0.5 etc.

        //m_ProjectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f); //Pixel based. Every unit is one pixel here.
        //m_ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));  //Moves our camera to the right, so everything else moves left on the screen.

        //We can see that we have successfully converted our vertex positions into that -1 to 1 space.
        //That is what projection does in both 2D and 3D, orthographic or perspective. All you're doing is telling your computer how to convert from whatever space you're dealing with (what you give it) to that -1 to 1 space.
        m_Shader = std::make_unique<Shader>("OpenGL/Shaders/Basic.shader");
        m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        m_Texture = std::make_unique<Texture>("Resources/Textures/PrismEngineLogo.png");
        m_SecondTexture = std::make_unique<Texture>("Resources/Textures/AeternumGameLogo.png");
        // texture.Bind();
         //shader.SetUniform1i("u_Texture", 0); //0 because we bound our texture to slot 0 in Texture.cpp.
                                              //Texture Coordinates tell our geometry which part of the texture to sample from. Our Fragment/Pixel shader goes through and rasterizes the rectangle,  
                                              //The fragment shader is responsible for the color of each pixel. We need to somehow tell the fragment shader to sample from the texture pixels to decide which color the pixel on the geometry will be.
                                              //We are to specify for each vertex we have on our rectangle, what area of the texture it should be. The frag shader will turn interpolate between that so that if we're rendering a pixel halfway between 2indices, it will choose a coordinate that is halfway through as well.  
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    TestTexture2D::~TestTexture2D()
    {
    }

    void TestTexture2D::OnUpdate(float deltaTime)
    {
    }

    void TestTexture2D::OnRender()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        OpenGLRenderer renderer;
        m_Texture->Bind();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
            //MVP - Model View Projection Matrix. Remember that this is in reverse because OpenGL's memory layout in its shader and GPU is column major, and that is why glm does this for us due to OpenGL.
            glm::mat4 mvp = m_ProjectionMatrix * m_ViewMatrix * model;
            m_Shader->Bind();
            m_Texture->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            m_Shader->SetUniform1i("u_Texture", 0);
            renderer.Draw(*m_VertexArrayObject, *m_IndexBuffer, *m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
            glm::mat4 mvp = m_ProjectionMatrix * m_ViewMatrix * model;
            m_Shader->Bind();
            m_SecondTexture->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            m_Shader->SetUniform1i("u_Texture", 0);
            renderer.Draw(*m_VertexArrayObject, *m_IndexBuffer, *m_Shader);
        }
    }

    void TestTexture2D::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 960.0f);
        ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 960.0f);
    }
}