#include "GAAPrecompiledHeader.h"
#include "Renderer.h"
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "OpenGL/OpenGLRenderer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexBuffer.h"

///Excuse the mess! :)

//A shader is a program that runs on the GPU. We do this because we want to be able to tell the GPU what to do. It is extremely powerful.
//All forms of information such as vertex position, color and textures originally given by us on the CPU are packed into a shader and sent to the GPU to be rendered. 
//We go from the draw call to the vertex shader, to the fragment shader to the draw call and eventually a triangle is seen.

//A Vertex Shader is ran once per vertex. This tells OpenGL where we want the vertex to be on the screen space. It also passes data from attributes into the next stage. In our case, we have position.
//A fragment shader is run once for each pixel that needs to be rasterized. It decides what color each pixel is supposed to be. Thus, when a triangle has each vertex position decided, the fragment shader fills in the gap between the 3 positions for the triangle.
//If there are certain things you can do, try doing it in the Vertex Shader instead of a fragment shader. That is because running something 3 times per vertex vs 3000 times per pixel has a huge difference.
//Note that like everything else in OpenGL, we have to enable a shader to use it. 

//Vertex Buffers allow us to submit vertex data to the GPU via attributes.
//Uniforms also allow us to pass data from the CPU (C++ in this case) into the Shader so we can use it like a variable.
//Uniforms are set per draw before the actual drawing, while attributes are set via vertex.

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

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    enum class ShaderType
    {
        None = -1, Vertex = 0, Fragment = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::None;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos) //If we find the word #shader...
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::Vertex;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::Fragment;
            }
        }
        else
        {
            //The enum index here is used as the array index. Hence the above manual index assignments. Smart!
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); //i specifies we are specifying an integer, and v means it wants an array, aka a pointer.
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); //Alloca allows us to allocate on the stack dynamically. 
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << "\n";
        std::cout << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //Once linked, we can delete the intermediates.
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

        unsigned int vertexArrayObject;
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        VertexBuffer vertexBuffer(positions, 4 * 2 * sizeof(float));

        glEnableVertexAttribArray(0);
        //This is the line of code that binds the buffer to the vertex array. 
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        IndexBuffer indexBuffer(indices, 6);

        ShaderProgramSource source = ParseShader("OpenGL/Shaders/Basic.shader");

        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        glUseProgram(shader);  //We must have a shader bound before setting uniform data so that it knows which shader to send on to.
        //The difference between each uniform is the type of data we're sending and how many components we have. In this, case its a Vec4 aka 4 floats. 
        //When a shader is created, every uniform is assigned an ID which we can then reference. 
        //We reference it by name! :)
        int location = glGetUniformLocation(shader, "u_Color");
        ASSERT(location != -1);   //Ensure that our uniform can be found. 
        //Note that if we don't use the uniform, OpenGL strips it away when the shader is compiled. Thus, this may return -1 because the shader may have gotten removed due to such.

        //We have effectively moved the code from the shader into here, our C++ code. 
        glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f);

        glBindVertexArray(0);
        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shader);
            glUniform4f(location, r, 0.3f, 0.8f, 1.0f);
            glBindVertexArray(vertexArrayObject);
            indexBuffer.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); //We can put nullptr because the data is already bound to the buffer.

            if (r > 1.0f)
            {
                increment = -0.05f;
            }
            else if (r < 0.0f)
            {
                increment = 0.05f;
            }
            r += increment;
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;	
    //////////////////////
}