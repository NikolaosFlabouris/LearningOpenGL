#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Utility code to create and control a camera.
#include <Camera/camera.h>

// Utility code to load and compile GLSL shader programs.
#include <Shader/shader.h>

// Utility to load in images.
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "main.h"

#define VALS_PER_VERT 3
#define VALS_PER_COLOUR 4
#define VALS_PER_NORMAL 3
#define VALS_PER_TEX_COORD 2
#define NUM_VERTS 3

// Window size.
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Handle to our rectangle VAOs.
unsigned int rectangleVertexVaoHandle;

// Handle to our shader program.
Shader shaderIDCubeActive;
Shader shaderIDCubeDir = Shader();
Shader shaderIDCubePoint = Shader();
Shader shaderIDCubeSpot = Shader();
Shader shaderIDLight = Shader();

Camera camera;

// General camera variables.
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

unsigned int lightType = 0;

// Frame rate invariant timing.
float deltaTime = 0.0f;
float lastFrame = 0.0f;

///
/// Process all input by querying GLFW whether relevant keys are pressed/released
/// this frame and react accordingly.
///
/// \param window - the active GLFW Window.
///
void ProcessInput(GLFWwindow* window)
{
    float cameraSpeed = 2.5f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    // Escape key closes window.
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

///
/// Mouse callback, checks for mouse position for camera direction.
///
void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        // Grab first mouse position.
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }

    // Calculate change in mouse position and update last position.
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // Look sensitivity control.
    camera.ProcessMouseMovement(xoffset, yoffset);
}

///
/// Scroll callback simulates zoom by changing the FOV.
///
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Alter FOV to create zoom effect. Lock to certain bounds.
    camera.ProcessMouseScroll(yoffset);
}

///
/// Scroll callback simulates zoom by changing the FOV.
///
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        lightType++;
    }

    if(lightType % 3 == 0)
    {
        shaderIDCubeActive = shaderIDCubeDir;
    }
    else if(lightType % 3 == 1)
    {
        shaderIDCubeActive = shaderIDCubePoint;
    }
    else if(lightType % 3 == 2)
    {
        shaderIDCubeActive = shaderIDCubeSpot;
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
int SetCubeVertexData(Shader shaderID)
{
    // Set of 6 faces of a cube.
    float vertices[] =
    {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    // Texture coordinates for each vertex.
    float normals[] = {
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,

        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,

        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,

        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,

        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,

        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f
    };

    // Texture coordinates for each vertex.
    float texCoords[] = {
        0.0f, 0.0f, 
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
    };

    // Generate storage on the GPU for our triangle and make it current.
    // A VAO is a set of data buffers on the GPU.
    glGenVertexArrays(1, &rectangleVertexVaoHandle);
    glBindVertexArray(rectangleVertexVaoHandle);

    // Generate 2 new buffers in our VAO to store per-vertex attributes.
    // One buffer to store vertex positions.
    // One buffer to store normal data.
    unsigned int vertexBuffer;
    unsigned int normalBuffer;
    unsigned int textureBuffer;
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &normalBuffer);
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

    // --- NORMAL DATA

    // Allocate GPU memory for our normals and copy them over.
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    // Enable buffer and tell OpenGL how to interpret the data we just gave it.
    // Tell OpenGL what shader variable it corresponds to (location = 1)
    // and how it is formatted (floating point, 3 values per vertex).
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

    // --- TEXTURE DATA

    // Allocate GPU memory for our vertices and copy them over.
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    // Enable buffer and tell OpenGL how to interpret the data we just gave it.
    // Tell OpenGL what shader variable it corresponds to (location = 2)
    // and how it is formatted (floating point, 2 values per vertex).
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, VALS_PER_TEX_COORD, GL_FLOAT, GL_FALSE, 0, 0);

    // - Texture Diffuse

    // Load and generate the first texture.
    int width;
    int height;
    int numberOfChannels;
    glUseProgram(shaderID.ProgramID());

    // Generate a texture buffer in our VAO to store texture data.
    unsigned int texture1;
    glGenTextures(1, &texture1);

    // Allocate GPU memory for texture data to texture unit 0.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // Set the texture wrapping/filtering options (on the currently bound texture object).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Tell stb_image.h to flip loaded texture's on the y-axis.
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imageData1 = stbi_load("Textures/container2.png", &width, &height, &numberOfChannels, 0);
    if(imageData1)
    {
        GLenum format = GL_RGB;
        if(numberOfChannels == 1)
        {
            format = GL_RED;
        }
        else if(numberOfChannels == 3)
        {
            format = GL_RGB;
        }
        else if(numberOfChannels == 4)
        {
            format = GL_RGBA;
        }

        // If successfully read in texture data pass it to the buffer and generate mipmaps.
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(imageData1);

    // Bind uniform to texture.
    shaderID.SetUniformInt("material.diffuse", 0);

    // - Texture Specular

    // Generate a texture buffer in our VAO to store texture data.
    unsigned int texture2;
    glGenTextures(1, &texture2);

    // Allocate GPU memory for texture data to texture unit 1.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // Set the texture wrapping/filtering options (on the currently bound texture object).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* imageData2 = stbi_load("Textures/container2_specular.png", &width, &height, &numberOfChannels, 0);
    if(imageData2)
    {
        GLenum format = GL_RGB;
        if(numberOfChannels == 1)
        {
            format = GL_RED;
        }
        else if(numberOfChannels == 3)
        {
            format = GL_RGB;
        }
        else if(numberOfChannels == 4)
        {
            format = GL_RGBA;
        }

        // If successfully read in texture data pass it to the buffer and generate mipmaps.
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(imageData2);

    // Bind uniform to texture.
    shaderID.SetUniformInt("material.specular", 1);

    // An argument of zero unbinds all VAO's and stops us
    // from accidentally changing the VAO state.
    glBindVertexArray(0);

    // The same is true for buffers, so we unbind it too.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0;	// Return success.
}

///
/// Loads all the shaders.
///
void ShaderSetup()
{
    // Set up the shaders we are to use and use them. 0 indicates error.
    shaderIDCubeDir.LoadShaders("Shaders/litObject.vert", "Shaders/litObjectDirectional.frag");
    if(shaderIDCubeDir.ProgramID() == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    // Set the vertex data for a rectangle.
    if(SetCubeVertexData(shaderIDCubeDir) != 0)
    {
        std::cout << "Failed to set vertex data." << std::endl;
        exit(1);
    }

    // Set up the shaders we are to use and use them. 0 indicates error.
    shaderIDCubePoint.LoadShaders("Shaders/litObject.vert", "Shaders/litObjectPoint.frag");
    if(shaderIDCubePoint.ProgramID() == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    // Set the vertex data for a rectangle.
    if(SetCubeVertexData(shaderIDCubePoint) != 0)
    {
        std::cout << "Failed to set vertex data." << std::endl;
        exit(1);
    }

    // Set up the shaders we are to use and use them. 0 indicates error.
    shaderIDCubeSpot.LoadShaders("Shaders/litObject.vert", "Shaders/litObjectSpotlight.frag");
    if(shaderIDCubeSpot.ProgramID() == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    // Set the vertex data for a rectangle.
    if(SetCubeVertexData(shaderIDCubeSpot) != 0)
    {
        std::cout << "Failed to set vertex data." << std::endl;
        exit(1);
    }

    // Set up the shaders we are to use and use them. 0 indicates error.
    shaderIDLight.LoadShaders("Shaders/lightSource.vert", "Shaders/lightSource.frag");
    if(shaderIDLight.ProgramID() == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    shaderIDCubeActive = shaderIDCubeDir;
    glUseProgram(shaderIDCubeActive.ProgramID());
}

///
/// Sends the camera details to the shader.
///
void SendCameraDetails(Shader shaderID)
{
    // Create view transformation matrix.
    glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
    // Pass view transformation matrices to the shader
    shaderID.SetUniformMat4("view", view);

    // Apply new FOV to projection.
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), ( float) SCR_WIDTH / ( float) SCR_HEIGHT, 0.1f, 100.0f);
    shaderID.SetUniformMat4("projection", projection);
}

///
/// Sends information about the particular type of light caster to the shader.
///
void SendLightDetails(Shader shaderID)
{
    glUseProgram(shaderID.ProgramID());

    if(lightType % 3 == 0)
    {
        // --- DIRECTIONAL LIGHT

        // Set light direction.
        glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
        shaderID.SetUniformVec3("light.direction", lightDirection);

        // Set light colour.
        shaderID.SetUniformVec3("light.ambient", glm::vec3(0.2f));
        shaderID.SetUniformVec3("light.diffuse", glm::vec3(0.8f));
        shaderID.SetUniformVec3("light.specular", glm::vec3(1.0f));

        // Set material shininess.
        shaderID.SetUniformFloat("material.shininess", 32.0f);
    }
    else if(lightType % 3 == 1)
    {
        // --- POINT LIGHT
        // Set light position.
        glm::vec3 lightPosition = glm::vec3(0.0f, 1.0f, -2.0f);
        shaderID.SetUniformVec3("light.position", lightPosition);

        // Set light colour.
        shaderID.SetUniformVec3("light.ambient", glm::vec3(0.2f));
        shaderID.SetUniformVec3("light.diffuse", glm::vec3(0.8f));
        shaderID.SetUniformVec3("light.specular", glm::vec3(1.0f));

        // Set material shininess.
        shaderID.SetUniformFloat("material.shininess", 32.0f);

        // Set light attenuation.
        shaderID.SetUniformFloat("light.constant", 1.0f);
        shaderID.SetUniformFloat("light.linear", 0.09f);
        shaderID.SetUniformFloat("light.quadratic", 0.032f);

        // --- DRAW LIGHT OBJECT

        // Specify the shader program we want to use.
        glUseProgram(shaderIDLight.ProgramID());

        // Make the VAO with our vertex data buffer current.
        glBindVertexArray(rectangleVertexVaoHandle);

        SendCameraDetails(shaderIDLight);

        // Set light obj colour
        shaderIDLight.SetUniformVec3("colour", glm::vec3(1.0f));

        // Calculate the model matrix for each object and pass it to shader before drawing.
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPosition));
        model = glm::scale(model, glm::vec3(0.23f, 0.23f, 0.23f));
        shaderIDLight.SetUniformMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices per cube. 2 tris per face, 3 verts per tri.
    }
    else if(lightType % 3 == 2)
    {
        // --- SPOT LIGHT
        // Set light position.
        glm::vec4 lightPosition = glm::vec4(camera.Position, 1.0f);
        shaderID.SetUniformVec3("light.position", lightPosition);

        // Set light direction and angle.
        shaderID.SetUniformVec3("light.direction", camera.Front);
        shaderID.SetUniformFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shaderID.SetUniformFloat("light.outerCutOff", glm::cos(glm::radians(15.5f)));

        // Set light colour.
        shaderID.SetUniformVec3("light.ambient", glm::vec3(0.2f));
        shaderID.SetUniformVec3("light.diffuse", glm::vec3(0.8f));
        shaderID.SetUniformVec3("light.specular", glm::vec3(1.0f));

        // Set material shininess.
        shaderID.SetUniformFloat("material.shininess", 32.0f);

        // Set light attenuation.
        shaderID.SetUniformFloat("light.constant", 1.0f);
        shaderID.SetUniformFloat("light.linear", 0.09f);
        shaderID.SetUniformFloat("light.quadratic", 0.032f);
    }
    glUseProgram(shaderIDCubeActive.ProgramID());
}

///
/// Calculate and apply transformation matrix.
///
void ApplyTransformAndDraw(Shader shaderID)
{
    // World space positions of our cubes.
    glm::vec3 cubePositions[] =
    {
        glm::vec3(0.0f,  0.0f, -1.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // Render cubes.
    for(unsigned int cube = 0; cube < 10; ++cube)
    {
        // Calculate the model matrix for each object and pass it to shader before drawing.
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[cube]);
        float angle = 73.0f * cube;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shaderID.SetUniformMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices per cube. 2 tris per face, 3 verts per tri.
    }
}

///
/// Render, to be called every frame.
///
void Render()
{
    // Clear the previous pixels we have drawn to the colour buffer (display buffer)
    // and depth buffer. Called each frame so we don't draw over the top of everything previous.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- DRAW CUBE

    // Specify the shader program we want to use.
    glUseProgram(shaderIDCubeActive.ProgramID());

    // Make the VAO with our vertex data buffer current.
    glBindVertexArray(rectangleVertexVaoHandle);

    SendCameraDetails(shaderIDCubeActive);

    // Set the camera position.
    shaderIDCubeActive.SetUniformVec3("viewPos", camera.Position);

    SendLightDetails(shaderIDCubeActive);

    // Apply rotation, scale and/or translation send command to GPU to draw the data in the current VAO.
    ApplyTransformAndDraw(shaderIDCubeActive);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Light Casters, click to change", NULL, NULL);

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

    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);

    // Sets the (background) colour for each time the frame-buffer (colour buffer) is cleared
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // SHADER SETUP
    ShaderSetup();

    // Callbacks for camera control.
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    // Attach mouse click callback to change lightCaster.
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    // Capture mouse in window.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glUseProgram(shaderIDCubeActive.ProgramID());

    // The event loop, runs until the window is closed.
    // Each iteration redraws the window contents and checks for new events.
    // Windows are double buffered, so need to swap buffers.
    while(!glfwWindowShouldClose(window))
    {
        // Calculate deltaTime.
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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
