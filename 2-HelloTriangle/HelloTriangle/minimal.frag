#version 330

in vec4 colour; // The per-vertex, colour attribute from the previous stage.

// The final colour we will see at this location on screen
out vec4 fragColour;

void main(void)
{
    // We now just have to set the already smoothed colour as our frag colour    

	fragColour = colour;
}
