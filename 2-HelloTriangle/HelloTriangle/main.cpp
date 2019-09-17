#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Utility code to load and compile GLSL shader programs.
#include <Shader/shader.hpp>

#define VALS_PER_VERT 3
#define VALS_PER_COLOUR 4
#define NUM_VERTS 3

// Window size.
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Handle to our VAO generated in setShaderData method.
unsigned int vertexVaoHandle;

// Handle to our shader program.
unsigned int shaderID;

///
/// Process all input by querying GLFW whether relevant keys are pressed/released
/// this frame and react accordingly.
///
/// \param window - the active GLFW Window.
///
void ProcessInput(GLFWwindow* window)
{
    // Escape key closes window.
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

///
/// GLFW callback whenever the window size changed (by OS or user resize)
/// this callback function executes.
///
/// \param window - the GLFW Window that is producing the callback.
/// \param width - the new width of the window.
/// \param height - the new height of the window.
///
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Set the OpenGL viewport to be that of the new window size.
    glViewport(0, 0, width, height);
}

///
/// Sets the shader uniforms and vertex data. This happens ONCE only, before any frames are rendered.
///
/// \return - 0 for success, error otherwise
///
int SetVertexData()
{
    // Set of 3 vertices (9 floats) defining one triangle.
    float vertices[] = {
         -0.5f, -0.5f, 0.0f, // left  
          0.5f, -0.5f, 0.0f, // right 
          0.0f,  0.5f, 0.0f  // top   
    };

    // Colours to be rendered at each vertex.
    float colours[] = {
            1.0, 0.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0,
            0.0, 0.0, 1.0, 1.0
    };

    // Generate storage on the GPU for our triangle and make it current.
    // A VAO is a set of data buffers on the GPU.
    glGenVertexArrays(1, &vertexVaoHandle);
    glBindVertexArray(vertexVaoHandle);

    // Generate 2 new buffers in our VAO to store per-vertex attributes.
    // One buffer to store vertex positions and another to store colours, read by shaders.
    unsigned int vertexBuffer;
    unsigned int colourBuffer;
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &colourBuffer);

    // Allocate GPU memory for our vertices and copy them over.
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Now we tell OpenGL how to interpret the data we just gave it.
    // Tell OpenGL what shader variable it corresponds to (location = 0)
    // and how it is formatted (floating point, 3 values per vertex).
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Allocate GPU memory for our colours and copy them over.
    glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

    // Now we tell OpenGL how to interpret the data we just gave it.
    // Tell OpenGL what shader variable it corresponds to (location = 1)
    // and how it is formatted (floating point, 4 values per vertex).
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

    // An argument of zero unbinds all VAO's and stops us
    // from accidentally changing the VAO state.
    glBindVertexArray(0);

    // The same is true for buffers, so we unbind it too.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0;	// Return success.
}

///
/// Render, to be called every frame.
///
void Render()
{
    // Clear the previous pixels we have drawn to the colour buffer (display buffer)
    // and depth buffer. Called each frame so we don't draw over the top of everything previous.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Specify the shader program we want to use.
    glUseProgram(shaderID);

    // Make the VAO with our vertex data buffer current.
    glBindVertexArray(vertexVaoHandle);

    // Send command to GPU to draw the data in the current VAO as triangles.
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glFlush();	// Guarantees previous commands have been completed before continuing.
}

///
/// Error callback for GLFW. Simply prints error message to stderr.
///
static void ErrorCallback(int error, const char* description)
{
    fputs(description, stderr);
}

///
/// Renders a triangle in the centre of the screen. The 3 primary colours are assigned
/// to the vertices and the shader interpolates the internal colours from there.
///
int main(int argc, char** argv)
{
    glfwSetErrorCallback(ErrorCallback);

    // GLFW initialisation.
    if(!glfwInit())
    {
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Instantiate GLFW window with screen resolution and title.
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello Triangle", NULL, NULL);

    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Callback to change OpenGL viewport size as window size changes.
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Load all GLAD OpenGL function pointers.
    if(!gladLoadGLLoader(( GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Sets the (background) colour for each time the frame-buffer (colour buffer) is cleared
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set up the shaders we are to use. 0 indicates error.
    shaderID = ShaderUtils::LoadShaders("minimal.vert", "minimal.frag");
    if(shaderID == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    // Set the vertex data for the program
    if(SetVertexData() != 0)
    {
        std::cout << "Failed to set vertex data." << std::endl;
        exit(1);
    }

    // The event loop, runs until the window is closed.
    // Each iteration redraws the window contents and checks for new events.
    // Windows are double buffered, so need to swap buffers.
    while(!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);

    return 0;
}
