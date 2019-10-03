#version 330

// The final colour we will see at this location on screen.
out vec4 fragColour;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal; // The surface normal.
in vec3 fragPos; // World space location of the fragment.

uniform vec3 viewPos; // The position from which the camera is viewing the fragment.
uniform Material material;
uniform Light light;

void main(void)
{
	// Ambient light.
	vec3 ambient = light.ambient * material.ambient;

	// Diffuse light.
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// Specular light.
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);
	
    vec3 result = ambient + diffuse + specular;
	
	fragColour = vec4(result, 1.0f);
}
