#version 330

// Position and colour variables. 1 per vertex.
layout (location=0) in vec3 a_vertex;
layout (location=1) in vec4 a_colour;
layout (location=2) in vec2 a_tex_coord;

// This colour attribute will be sent out per-vertex and automatically
// interpolated between vertices connected to the same triangle
out vec4 colour;
out vec2 tex_coord;

uniform mat4 transform;

void main(void)
{
	colour = a_colour;	// We simply pass the colour along to the next stage
	tex_coord = a_tex_coord;

    // Multiply our vertex positions by the vertex transform set in main application
	gl_Position = transform * vec4(a_vertex, 1.0);
}
