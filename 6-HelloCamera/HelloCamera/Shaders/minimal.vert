#version 330

layout (location=0) in vec3 a_vertex;
layout (location=1) in vec2 a_tex_coord;

out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	tex_coord = a_tex_coord;

    // Multiply our vertex positions by the vertex transform set in main application
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
