#version 330 core

in vec2 TexCoords;

out vec4 color;

struct lightStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};


uniform lightStruct light;

uniform sampler2D texture_diffuse1;

void main()
{    

	// Ambient intensity
	vec4 ambientI = light.ambient;

	// Diffuse intensity
	vec4 diffuseI = light.diffuse;

	// Specular intensity
	vec4 specularI = light.specular;

	// Fragment colour
    color = (ambientI + diffuseI + specularI)*vec4(texture(texture_diffuse1, TexCoords));
}