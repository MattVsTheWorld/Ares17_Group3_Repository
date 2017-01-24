#version 330 core

in vec2 TexCoords;

out vec4 color;

struct lightStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct materialStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

in vec3 ex_N;
in vec3 ex_V;
in vec3 ex_L;

uniform lightStruct light;
uniform materialStruct material;

uniform sampler2D texture_diffuse1;

void main()
{    

	// Ambient intensity
	vec4 ambientI = light.ambient * material.ambient;

	// Diffuse intensity
	vec4 diffuseI = light.diffuse * material.diffuse;
	diffuseI = diffuseI * max(dot(normalize(ex_N),normalize(ex_L)),0);

	// Specular intensity
	vec3 R = normalize(reflect(normalize(-ex_L),normalize(ex_N)));
	vec4 specularI = light.specular * material.specular;
	specularI = specularI * pow(max(dot(R,ex_V),0), material.shininess);

	// Fragment colour
    color = (ambientI + diffuseI + specularI)*vec4(texture(texture_diffuse1, TexCoords));
}