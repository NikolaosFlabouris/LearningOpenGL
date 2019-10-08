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
Shader shaderIDCube = Shader();
Shader shaderIDLight = Shader();

Camera camera;

// General camera variables.
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Alter FOV to create zoom effect. Lock to certain bounds.
    camera.ProcessMouseScroll(yoffset);
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
int SetCubeVertexData()
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
    glUseProgram(shaderIDCube.ProgramID());

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
    shaderIDCube.SetUniformInt("material.diffuse", 0);

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
    shaderIDCube.SetUniformInt("material.specular", 1);

    // An argument of zero unbinds all VAO's and stops us
    // from accidentally changing the VAO state.
    glBindVertexArray(0);

    // The same is true for buffers, so we unbind it too.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0;	// Return success.
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
/// Render, to be called every frame.
///
void Render()
{
    // Clear the previous pixels we have drawn to the colour buffer (display buffer)
    // and depth buffer. Called each frame so we don't draw over the top of everything previous.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 lightPos = glm::vec3(-0.4f, 0.75f, -1.5f);

    // --- DRAW CUBE

    // Specify the shader program we want to use.
    glUseProgram(shaderIDCube.ProgramID());

    // Make the VAO with our vertex data buffer current.
    glBindVertexArray(rectangleVertexVaoHandle);

    SendCameraDetails(shaderIDCube);

    // Set light position.
    shaderIDCube.SetUniformVec3("light.position", lightPos);
    // Set light colour.
    glm::vec3 lightColor = glm::vec3(abs(sin(glfwGetTime() * 1.4f)), abs(sin(glfwGetTime() * 0.7f)), abs(sin(glfwGetTime() * 1.1f)));
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
    shaderIDCube.SetUniformVec3("light.ambient", ambientColor);
    shaderIDCube.SetUniformVec3("light.diffuse", diffuseColor);
    shaderIDCube.SetUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // Set the camera position.
    shaderIDCube.SetUniformVec3("viewPos", camera.Position);

    // Set material shininess.
    shaderIDCube.SetUniformFloat("material.shininess", 32.0f);

    // Calculate the model matrix for each object and pass it to shader before drawing.
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, 0.0, -4.0));
    model = glm::rotate(model, (float) glm::radians(73.0f * sin(glfwGetTime() * 0.6f)), glm::vec3(1.0f, 0.3f, 0.5f));
    shaderIDCube.SetUniformMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices per cube. 2 tris per face, 3 verts per tri.

    // --- DRAW LIGHT

    // Specify the shader program we want to use.
    glUseProgram(shaderIDLight.ProgramID());

    // Make the VAO with our vertex data buffer current.
    glBindVertexArray(rectangleVertexVaoHandle);

    SendCameraDetails(shaderIDLight);

    // Set light obj colour
    shaderIDLight.SetUniformVec3("colour", lightColor);

    // Calculate the model matrix for each object and pass it to shader before drawing.
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.23f, 0.23f, 0.23f));
    shaderIDLight.SetUniformMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices per cube. 2 tris per face, 3 verts per tri.

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Maps", NULL, NULL);

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

    // Set up the shaders we are to use and use them. 0 indicates error.
    shaderIDCube.LoadShaders("Shaders/litObject.vert", "Shaders/litObject.frag");
    if(shaderIDCube.ProgramID() == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    // Set up the shaders we are to use and use them. 0 indicates error.
    shaderIDLight.LoadShaders("Shaders/lightSource.vert", "Shaders/lightSource.frag");
    if(shaderIDLight.ProgramID() == 0)
    {
        std::cout << "Failed to load shaders." << std::endl;
        exit(1);
    }

    // Set the vertex data for a rectangle.
    if(SetCubeVertexData() != 0)
    {
        std::cout << "Failed to set vertex data." << std::endl;
        exit(1);
    }

    // Callbacks for camera control.
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture mouse in window.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
