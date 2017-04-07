#version 330 core

out vec4 FragColor;

//show weights
in vec4 we;
in int anim; //to show wieghts or not

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
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

uniform PointLight pointLight;
uniform Material material;
uniform samplerCube depthMap;
uniform vec3 viewPos;
uniform float far_plane;

// Function prototype
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow);

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float ShadowCalculation(vec3 fragPos, vec3 lightPosition, samplerCube shadowCube)
{

    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPosition;
    // Use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(shadowCube, fragToLight).r;
    // It is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Now test for shadows
    float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
	//apply PCF to soften shadows, using sampleOffsetDirections to reduce the number of samples taken
	//diskRadius is used to increase the offset radius by the distance to the viewer
	//This makes the shadows softer when far away and sharper when close by
	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowCube, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;   // Undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);  

    return shadow;
}

void main()
{    
    vec3 normal = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	float shadow = ShadowCalculation(fs_in.FragPos, pointLight.position, depthMap);
	vec3 result = CalcPointLight(pointLight, normal, fs_in.FragPos, viewDir, shadow);  
	//show weights
	if(anim == 2) {
		vec4 weightsColor = vec4(we.xyz,1.0);
		FragColor = weightsColor;
	}
	else {
		FragColor = vec4(result, 1.0f);
	}	
}  


// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + (diffuse + specular) * (1.0 - shadow));
}
