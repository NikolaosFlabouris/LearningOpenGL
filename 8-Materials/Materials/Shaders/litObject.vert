#version 330

layout (location=0) in vec3 a_vertex;
layout (location=1) in vec3 a_normal;

out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	normal = mat3(transpose(inverse(model))) * a_normal; // Note: best to calculate this on CPU then send to GPU.
	fragPos = vec3(model * vec4(a_vertex, 1.0));

    // Multiply our vertex positions by the vertex transform set in main application
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
