//#define GLFW_INCLUDE_NONE

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
unsigned int programID;


///
/// Process all input by querying GLFW whether relevant keys are pressed/released
/// this frame and react accordingly.
///
/// \param window - the active GLFW Window.
///
void processInput(GLFWwindow* window)
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
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Set the OpenGL viewport to be that of the new window size.
    glViewport(0, 0, width, height);
}

/**
 * Sets the shader uniforms and vertex data
 * This happens ONCE only, before any frames are rendered
 * @returns 0 for success, error otherwise
 */
int setVertexData()
{
    /*
     * What we want to draw
     * Each set of 3 vertices (9 floats) defines one triangle
     * You can define more triangles to draw here
     */
    float vertices[] = {
            -0.5f, -0.5f, -0.0f,	// Bottom left
            0.5f, -0.5f, -0.0f,		// Bottom right
            0.0f, 0.5f, -0.0f		// Top middle
    };

    float colours[] = {
            1.0, 0.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0,
            0.0, 0.0, 1.0, 1.0
    };

    // Generate storage on the GPU for our triangle and make it current.
    // A VAO is a set of data buffers on the GPU.
    glGenVertexArrays(1, &vertexVaoHandle);
    glBindVertexArray(vertexVaoHandle);

    // Generate new buffers in our VAO
    // A single data buffer store for per-vertex attributes (e.g. position).
    unsigned int buffer[2];
    glGenBuffers(2, buffer);

    // Allocate GPU memory for our vertices and copy them over.
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Now we tell OpenGL how to interpret the data we just gave it.
    // Tell OpenGL what shader variable it corresponds to.
    // Tell OpenGL how it's formatted (floating point, 3 values per vertex).
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

    // An argument of zero unbinds all VAO's and stops us
    // from accidentally changing the VAO state.
    glBindVertexArray(0);

    // The same is true for buffers, so we unbind it too.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0;	// return success
}

/**
 * Renders a frame of the state and shaders we have set up to the window
 * Executed each time a frame is to be drawn.
 */
void render()
{
    // Clear the previous pixels we have drawn to the colour buffer (display buffer)
    // Called each frame so we don't draw over the top of everything previous
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Specify the shader program we want to use
    glUseProgram(programID);

    // Make the VAO with our vertex data buffer current
    glBindVertexArray(vertexVaoHandle);

    // Send command to GPU to draw the data in the current VAO as triangles
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);

    glBindVertexArray(0);	// Un-bind the VAO

    glFlush();	// Guarantees previous commands have been completed before continuing
}

/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

/**
 * Program entry. Sets up OpenGL state, GLSL Shaders and GLFW window and function call backs
 * Takes no arguments
 */
int main(int argc, char** argv)
{
    glfwSetErrorCallback(error_callback);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load all GLAD OpenGL function pointers.
    if(!gladLoadGLLoader(( GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Sets the (background) colour for each time the frame-buffer (colour buffer) is cleared
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set up the shaders we are to use. 0 indicates error.
    programID = LoadShaders("minimal.vert", "minimal.frag");
    if(programID == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    // Set the vertex data for the program
    if(setVertexData() != 0)
    {
        std::cout << "Failed to set vertex data." << std::endl;
        exit(1);
    }

    // The event loop, runs until the window is closed.
    // Each iteration redraws the window contents and checks for new events.
    // Windows are double buffered, so need to swap buffers.
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);

    return 0;
}
