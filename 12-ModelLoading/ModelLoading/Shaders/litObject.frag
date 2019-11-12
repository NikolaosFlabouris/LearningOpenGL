#version 330

// The final colour we will see at this location on screen.
out vec4 fragColour;

struct Material {
    sampler2D diffuse; // used as ambient colour as well.
    sampler2D specular;    
    float shininess;
}; 

struct DirLight {

    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NUM_POINT_LIGHTS 4

struct SpotLight {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};  

in vec3 normal; // The surface normal.
in vec3 fragPos; // World space location of the fragment.
in vec2 texCoords; // The frag location on the texture.

uniform vec3 viewPos; // The position from which the camera is viewing the fragment.
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading.
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading.
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results.
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading.
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading.
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation.
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // Combine results.
    vec3 ambient = light.ambient  * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse  * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);  
	// Diffuse shading.
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular shading..
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
	  
    // Spotlight (soft edges).
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;
    
    // Attenuation.
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;  

    return (ambient + diffuse + specular);
}

void main(void)
{
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	// Directional Light.
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	// Point Lights.
	for(int light = 0; light < NUM_POINT_LIGHTS; ++light)
	{
		result += CalcPointLight(pointLights[light], norm, fragPos, viewDir);
	}

	// Spot Light.
	result += CalcSpotLight(spotLight, norm, fragPos, viewDir);
	
	fragColour = vec4(result, 1.0f);
}
