#version 330 core

// Based on and adapted from the point shadows tutorial by http://learnopengl.com/
// Available: https://learnopengl.com/#!Advanced-Lighting/Shadows/Point-Shadows
// [Accessed: December 2016]

in vec4 FragPos;

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int currentLight;
uniform float far_plane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - pointLights[currentLight].position);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // Write this as modified depth
    gl_FragDepth = lightDistance;
}  