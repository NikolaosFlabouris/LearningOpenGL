#version 330

// Position and colour variables. 1 per vertex.
layout (location=0) in vec3 a_vertex; 
layout (location=1) in vec4 a_colour;	

// This colour attribute will be sent out per-vertex and automatically
// interpolated between vertices connected to the same triangle
out vec4 colour;

uniform mat4 transform;

void main(void)
{
	colour = a_colour;	// We simply pass the colour along to the next stage

    // Multiply our vertex positions by the vtx transform set in main application
	gl_Position = vec4(a_vertex, 1.0);
}
