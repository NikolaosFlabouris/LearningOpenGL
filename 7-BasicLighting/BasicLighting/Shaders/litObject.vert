#version 330

layout (location=0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

out vec3 normal;
out vec3 objColour;
out vec3 lightCol;
out vec3 lightPos;
out vec3 fragPos;
out vec3 viewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 colour;
uniform vec3 lightColour;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main(void)
{
	normal = mat3(transpose(inverse(model))) * a_normal; // Note: best to calculate this on CPU then send to GPU.
	objColour = colour;
	lightCol = lightColour;
	lightPos = lightPosition;
	fragPos = vec3(model * vec4(a_vertex, 1.0));

    // Multiply our vertex positions by the vertex transform set in main application
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
