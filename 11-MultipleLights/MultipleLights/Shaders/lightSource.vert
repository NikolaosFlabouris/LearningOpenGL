#version 330

layout (location=0) in vec3 a_vertex;

out vec2 tex_coord;
out vec3 objColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 colour;

void main(void)
{
	objColour = colour;

    // Multiply our vertex positions by the vertex transform set in main application
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
