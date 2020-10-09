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
#include "Tests/TestTexture2D.h"
#include "LearnShader.h"
#include "stb_image/stb_image.h"

//Settings
unsigned int m_ScreenWidth = 800;
unsigned int m_ScreenHeight = 600;
float visibleValue = 0.1f;

//This is a callback function that is called whenever a window is resized.
void FramebufferResizeCallback(GLFWwindow* window, int windowWidth, int windowHeight)
{
    glViewport(0, 0, windowWidth, windowHeight);
    m_ScreenHeight = windowHeight;
    m_ScreenWidth = windowWidth; 
}

/// ===== Input =====

//We also want some form of input control in GLFW.
//We can use GLFW's "glfwGetKey()" function that takes the window as input together with a key.
//The function returns whether said key is currently being pressed. We're creating a "ProcessInput" function to keep all input code organized.
//This gives us an easy way to check for specific key presses and react accordingly every frame. An iteration of a render loop is more commonly called a frame. 
void ProcessInput(GLFWwindow* window, LearnShader shaderProgram)
{
    //Here, we check whether the user has pressed the escape key (if not pressed, "glfwGetKey()" returns GLFW_RELEASE).
    //If the user did press the escape key, we close GLFW by setting its WindowShouldClose property to true using "glfwSetWindowShouldClose()". 
    //The next condition check of the main while loop will then fail and the application closes.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        visibleValue = visibleValue + 0.05f;
        unsigned int uniformLocation = glGetUniformLocation(shaderProgram.GetShaderID(), "textureViewValue");
        glUniform1f(uniformLocation, visibleValue);
    }

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		visibleValue = visibleValue - 0.05f;
		unsigned int uniformLocation = glGetUniformLocation(shaderProgram.GetShaderID(), "textureViewValue");
		glUniform1f(uniformLocation, visibleValue);
	}
}

int main()
{
    std::cout << "Start of Program!" << "\n";
    RendererAbstractor::Renderer::InitializeSelectedRenderer(RendererAbstractor::Renderer::API::OpenGL);

    /// ===== Hello Window =====

    glfwInit();
    //Refer to https://www.glfw.org/docs/latest/window.html#window_hints for a list of window hints avaliable here. This goes into the first value of glfwWindowHint.
    //We are essentially setting the value of the enum (first value) to the second value.
    //Here, we are telling GLFW that 3.3 is the OpenGL version we want to use. This allows GLFW to make the proper arrangements when creating the OpenGL context.
    //Thus, when users don't have the proper OpenGL version on his or her computer that is lower than 3.3, GLFW will fail to run, crash or display undefined behavior. 
    //We thus set the major and minor version to both 3 in this case.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //We also tell GLFW that we explicitly want to use the core profile. This means we will get access to a smaller subset of OpenGL features without backwards compatible features we no longer need.
    //On Mac OS X, you need to add "glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);" for it to work.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   
    //Creates a Window Object. This window object holds all the windowing data and is required by most of GLFW's other functions. 
    //The "glfwCreateWindow()" function requires the window width and height as its first two arguments respectively.
    //The third argument allows us to create a name for the window which I've called "OpenGL".
    //We can ignore the last 2 parameters. The function returns a GLFWwindow object that we will later need for other GLFW operations.
    GLFWwindow* window = glfwCreateWindow(m_ScreenWidth, m_ScreenHeight, "OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW Window! \n";
        glfwTerminate();
        return -1;
    }
    //We tell GLFW to make the context of our window the main context on the current thread.
    glfwMakeContextCurrent(window);
    //Sets a callback to a viewport resize function everytime we resize our window.
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

    //There are many callbacks we can set to register our own functions. This ranges from callbacks that process input, error messages etc. 
    //We register these callbacks after we've created the window and before the render loop is initiated. 

    //Initializes GLEW. 
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }

    LearnShader ourShader("VertexShader.shader", "FragmentShader.shader");

    //OpenGL doesn't simply transform all of your 3D coordinates to 2D pixels on the screen. 
    //OpenGL only processes 3D coordinates when they're in a specific range between -1.0 and 1.0 on all 3 axes (x, y and z).
    //All coordinates within this so called normalized device coordinates range will end up being visible on your screen, and all outside this region won't.
    //As we want to render a single triangle, we first specify a total of 3 vertices, with each vertex having a 3D position. 
    //We define them in normalized device coordinates (the visible region of OpenGL) in a float array.
    
    //Because OpenGL works in 3D space, we render a 2D triangle with each vertex having a Z coordinate of 0.0. This way, the depth of the triangle remains the same, making it look like its 2D. 
    float vertices[] =
    {
		// positions          // colors           // texture coordinates
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
        // positions         // colors
        //0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        //-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        // 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 

    };

    unsigned int indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    //Creates a vertex array object that takes in a vertex buffer and attrib pointers.
    unsigned int vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject); //Every subsequent attribute pointer call will now link the buffer and said attribute configurations to this vertex array object.



    //With the vertex data defined, we would like to send it as input to the first process of the graphics pipeline: the vertex shader.
    //This is done by creating memory on the GPU where we store the vertex data, configure how OpenGL should interpret the memory and specify how to send data to the graphics card.
    //The vertex shader then processes as much vertices as we tell it to from its memory.

    //We manage this memory via so called vertex buffer objects (VBO) that can store a large number of vertices in the GPU's memory.
    //The advantage of using those buffer objects is that we can send large batches of data all at once to the graphics card, and keep it there if there is enough memory left, without having to send data one vertex at a time.
    //Sending data to the graphics card from the CPU is relatively slow, so wherever we can we try to send as much data as possible at once.
    //Once the data is in the graphics card's memory, the vertex shader has almost instant access to the vertices, making it extremely fast. 


          //A Vertex Buffer Object is our first occurance of an OpenGL object. Just like any object in OpenGL, thus buffer has a unique ID corresponding to that buffer. We can generate one with a buffer ID using the "glGenBuffers()" function.
    //OpenGL has many types of buffer objects and the buffer type of a vertex buffer object is GL_ARRAY_BUFFER. OpenGL allows us to bind several buffers at once as long as they have a different buffer type.
    //We can bind the newly created buffer to the GL_ARRAY_BUFFER target using the "glBindBuffer" function.
    unsigned int vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    //From this point on, any buffer calls we make on the GL_ARRAY_BUFFER target will be used to configure the currently bound buffer, which is VertexBufferObject.
    //Then, we can make a call to the "glBufferData()" function that copies the previously defined vertex data into the buffer's memory.
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    //"glBufferData()" is a function specifically targeted to copy user-defined data into the currently bound buffer.
    //Its first argument is the type of buffer we want to copy data into: the vertex buffer object currently bound to the GL_ARRAY_BUFFER target.
    //The second argument specifies the size of the data in bytes that we want to pass to the buffer; a single sizeof of the vertex data suffices.
    //The third parameter is the actual data we want to send.
    //The fourth parameter specifies how we want the graphics card to manage the given data. This can take 3 forms.
    // - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    // - GL_STATIC_DRAW: the data is set only once and used many times.
    // - GL_DYNAMIC_DRAW: the data is changed a lot and use many times.
    //The position of the data does not change, is used a lot and stays the same every render call, so its usage type should best be GL_STATIC_DRAW.
    //If, for instance, one would have a buffer with data that is likely to change frequently, a usage type of GL_DYNAMIC_DRAW ensures the graphics card will place the data in memory that allows for faster writes.
    //As of now, we have stored the vertex data within memory on the graphics card as managed by a Vertex Buffer Object named VBO. Next, we want to create a vertex and fragment shader that actually processes this data, so lets start building those. 

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Index Buffer Creation
	unsigned int indexBufferObject;
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //Links the vertex attributes from the buffers that we passed into the shaders.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);



    unsigned int textureData1, textureData2;
    glGenTextures(1, &textureData1);
    glBindTexture(GL_TEXTURE_2D, textureData1);
    //Sets the texture wrapping and filtering options on the currently bound texture object. 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //When objects are zoomed out aka scaled down (further away), we interpolate from the texel closest to the fragment. 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //When objects are zoomed in aka scaled up, we interpolate from a combination of nearest texels to the fragment.
   
 //Load and generate the texture.
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("Resources/Textures/Container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load textures! \n";
    }
    stbi_image_free(data);

    glGenTextures(1, &textureData2);
    glBindTexture(GL_TEXTURE_2D, textureData2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("Resources/Textures/AwesomeFace.png", &width, &height, &nrChannels, 0);
    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data2);
    

    //unsigned int colorUniformLocation = glGetUniformLocation(shaderProgram, "ourColor");
    //glUseProgram(shaderProgram);
    ourShader.UseShader(); //Always activate shaders before setting uniforms.
    ourShader.SetUniformInteger("texture1", 0);
    ourShader.SetUniformInteger("texture2", 1);

    //We don't want the application to draw a single image and than just quit and close the window.
    //We want to program to keep drawing images and handling user input until it has been explicitly told to stop.
    //For this reason, we have to create a while loop, that we now call the render loop that keeps on running until we tell GLFW to stop.
    
    //The "glfwWindowShouldClose()" function checks at the start of each loop iteration if GLFW has been instructed to close. If so, the function returns true and the render loop stops running, after which we can close the application.
    //The "glfwPollEvents()" function checks if any events are triggered (like keyboard input or mouse movement events), updates the window state and calls the corresponding functions which we can register via callback methods.
    //The "glfwSwapBuffers()" function will swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window) that is used to render to during this render iteration and show it as output on the screen.

    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window, ourShader); //Process key input events.

        /// ===== Rendering =====

        //We want to place all the rendering commands in the render loop, since we want to execute all the rendering commands each iteration or frame of the loop. 
        //Lets clear the screen with a color of our choice. At the start of each frame, we want to clear the screen.
        //Otherwise, we would still see results from the previous frame, which could of course be the effect you're looking for, but usually, we don't.
        //We can clear the screen's color buffer using "glClear()", where we pass in buffer bits to specify which buffer we would like to clear.
        //The possible bits we can set are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT. Right now, we only care about the color values, so we only clear the color buffer.
        
        //Note that we also specify the color to clear the screen with using "glClearColor()".
        //Whenever we call "glClear()" and clear the color buffer, the entire color buffer will be filled with the color as configured with "glClearColor()".
        //As you may recall, the "glClearColor()" function is a state setting function and "glClear()" is a state using function in that it uses the current state to retrieve the clear color from.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //float timeValue = glfwGetTime();
        //float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        //glUniform4f(colorUniformLocation, 0.5f, greenValue, 0.4f, 0.4f);
        //Draws Triangle
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureData1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureData2);

        ourShader.UseShader();
		glBindVertexArray(vertexArrayObject); //Binds the buffer and configurations for the object we want to draw (provided we have binded it to something else before calling this).

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
    ourShader.DeleteShader();

    //As we exit the render loop, remember to properly clean and delete all of GLFW's resources that were allocated.
    //We can do this via the "glfwTerminate()" function that we call at the end of the main function.
    //This will cleanup all the resources and properly exit the application. 
    glfwTerminate();
    return 0;
}


/*
* 
    //glfwSwapInterval(1);



{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    OpenGLRenderer renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImFont* pFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    io.FontDefault = io.Fonts->Fonts.back();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls

    //Try making these tests command line arguments! :)
    //You can create test menus on the get go and they are more "user specific" = won't clutter github.
    Test::Test* currentTest = nullptr;
    Test::TestMenu* testMenu = new Test::TestMenu(currentTest);
    currentTest = testMenu;
    testMenu->RegisterTest<Test::TestClearColor>("Clear Color");
    testMenu->RegisterTest<Test::TestTexture2D>("2D Texture");

    /* bool show_demo_window = true;
     bool show_another_window = false;
     ImVec4 clear_color = ImVec4(0.45, 0.55f, 0.60f, 1.00f);*/

     /* Loop until the user closes the window 
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        /* Render here 
        renderer.Clear();

        //New Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (currentTest != nullptr)
        {
            currentTest->OnUpdate(0.0f);
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-"))
            {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }

        {
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
            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        //Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        /* Swap front and back buffers 
        glfwSwapBuffers(window);

        /* Poll for and process events 
        glfwPollEvents();
    }
    delete currentTest;
    if (currentTest != testMenu)
    {
        delete testMenu;
    }
}

ImGui_ImplOpenGL3_Shutdown();
ImGui_ImplGlfw_Shutdown();
ImGui::DestroyContext();
glfwTerminate();
return 0;
////////////////////


//
*/