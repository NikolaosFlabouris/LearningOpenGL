#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Utility code to load and compile GLSL shader programs.
#include <Shader/shader.hpp>

// Utility to load in images.
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define VALS_PER_VERT 3
#define VALS_PER_COLOUR 4
#define VALS_PER_TEX_COORD 2
#define NUM_VERTS 3

// Window size.
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Handle to our rectangle VAOs.
unsigned int rectangleVertexVaoHandle;

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
/// Sets the shader uniforms and rectangle vertex data. This happens ONCE only, before any frames are rendered.
///
/// \return - 0 for success, error otherwise
///
int SetRectangleVertexData()
{
    // Set of 4 vertices (12 floats) defining two triangles.
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left   
    };

    // Indices that define two triangles using the vertices.
    unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    // Colours to be rendered at each vertex.
    float colours[] = {
            1.0, 0.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0,
            0.0, 0.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0
    };

    // Texture coordinates for each vertex.
    float texCoords[] = {
        1.0f, 1.0f,  // top right  
        1.0f, 0.0f,  // bottom right
        0.0f, 0.0f,  // bottom left
        0.0f, 1.0f,  // top left
    };

    // Generate storage on the GPU for our triangle and make it current.
    // A VAO is a set of data buffers on the GPU.
    glGenVertexArrays(1, &rectangleVertexVaoHandle);
    glBindVertexArray(rectangleVertexVaoHandle);

    // Generate 3 new buffers in our VAO to store per-vertex attributes.
    // One buffer to store vertex positions.
    // One buffer to store colours, read by shaders.
    // One buffer to store the vertices array indices that form the triangle shapes.
    unsigned int vertexBuffer;
    unsigned int colourBuffer;
    unsigned int elementBuffer;
    unsigned int textureBuffer;
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &colourBuffer);
    glGenBuffers(1, &elementBuffer);
    glGenBuffers(1, &textureBuffer);

    // --- VERTEX DATA

    // Allocate GPU memory for our vertices and copy them over.
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Enable buffer and tell OpenGL how to interpret the data we just gave it.
    // Tell OpenGL what shader variable it corresponds to (location = 0)
    // and how it is formatted (floating point, 3 values per vertex).
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // --- COLOUR DATA

    // Allocate GPU memory for our colours and copy them over.
    glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

    // Enable buffer and tell OpenGL how to interpret the data we just gave it.
    // Tell OpenGL what shader variable it corresponds to (location = 1)
    // and how it is formatted (floating point, 4 values per vertex).
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

    // --- ELEMENT DATA

    // Allocate GPU memory for our indices and copy them over.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // --- TEXTURE DATA

    // - Texture Coordinates

    // Allocate GPU memory for our vertices and copy them over.
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    
    // Enable buffer and tell OpenGL how to interpret the data we just gave it.
    // Tell OpenGL what shader variable it corresponds to (location = 2)
    // and how it is formatted (floating point, 2 values per vertex).
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, VALS_PER_TEX_COORD, GL_FLOAT, GL_FALSE, 0, 0);

    // - Texture 1

    // Generate a texture buffer in our VAO to store texture data.
    unsigned int texture1;
    glGenTextures(1, &texture1);

    // Allocate GPU memory for texture data to texture unit 0.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // Set the texture wrapping/filtering options (on the currently bound texture object).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and generate the first texture.
    int width;
    int height;
    int numberOfChannels;

    // Tell stb_image.h to flip loaded texture's on the y-axis.
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imageData1 = stbi_load("Textures/container.jpg", &width, &height, &numberOfChannels, 0);
    if(imageData1)
    {
        // If successfully read in texture data pass it to the buffer and generate mipmaps.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(imageData1);

    // Bind uniform to texture.
    glUniform1i(glGetUniformLocation(shaderID, "inputTexture1"), 0);

    // - Texture 2

    // Generate a texture buffer in our VAO to store texture data.
    unsigned int texture2;
    glGenTextures(1, &texture2);

    // Allocate GPU memory for texture data to texture unit 0.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // Set the texture wrapping/filtering options (on the currently bound texture object).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and generate the second texture.
    unsigned char* imageData2 = stbi_load("Textures/awesomeface.png", &width, &height, &numberOfChannels, 0);
    if(imageData2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(imageData2);

    // Bind uniform to texture.
    glUniform1i(glGetUniformLocation(shaderID, "inputTexture2"), 1);

    // An argument of zero unbinds all VAO's and stops us
    // from accidentally changing the VAO state.
    glBindVertexArray(0);

    // The same is true for buffers, so we unbind it too.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0;	// Return success.
}

///
/// Calculate and apply transformation matrix.
///
void ApplyTransform1()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, (float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
    transform = glm::rotate(transform, (float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.0f));
    
    unsigned int transformLoc = glGetUniformLocation(shaderID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
}

///
/// Calculate and apply transformation matrix.
///
void ApplyTransform2()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, -(float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    float scaleAmount = 0.15f + (0.15f + 0.15f * sin(3.0f * (float) glfwGetTime()));
    transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, 0.0f));

    unsigned int transformLoc = glGetUniformLocation(shaderID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
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
    glBindVertexArray(rectangleVertexVaoHandle);

    // Apply 1st rotation, scale and/or translation.
    ApplyTransform1();

    // Send command to GPU to draw the data in the current VAO as triangles.
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Apply 2nd rotation, scale and/or translation.
    ApplyTransform2();

    // Send command to GPU to draw the data in the current VAO as triangles.
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello Transform", NULL, NULL);

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
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Sets the (background) colour for each time the frame-buffer (colour buffer) is cleared
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set up the shaders we are to use and use them. 0 indicates error.
    shaderID = ShaderUtils::LoadShaders("Shaders/minimal.vert", "Shaders/minimal.frag");
    if(shaderID == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    glUseProgram(shaderID);

    // Set the vertex data for a rectangle.
    if(SetRectangleVertexData() != 0)
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
