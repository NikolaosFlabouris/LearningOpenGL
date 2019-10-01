#version 330

in vec3 objColour; // The colour of the object.

// The final colour we will see at this location on screen.
out vec4 fragColour;

void main(void)
{
	fragColour = vec4(objColour, 1.0);
}
