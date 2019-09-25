#version 330

in vec4 colour; // The per-vertex, colour attribute from the previous stage.
in vec2 tex_coord; // The per vertex, texture coordinate attribute from the previous stage.

// The final colour we will see at this location on screen.
out vec4 fragColour;

uniform sampler2D inputTexture1; // declared 1st so texture unit 0
uniform sampler2D inputTexture2; // declared 2nd so texture unit 1

void main(void)
{
    // Set fragment colour to be the colour of the texture at the interpolated coordinate.    
	fragColour = mix(texture(inputTexture1, tex_coord), texture(inputTexture2, tex_coord), 0.2) * colour;
	//fragColour = texture(inputTexture1, tex_coord);
}
