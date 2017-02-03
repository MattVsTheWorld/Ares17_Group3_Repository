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

uniform sampler2D texture_diffuse[1];

//uniform sampler2D texture_specular[1];

void main()
{    

	// Ambient intensity
	vec4 ambientI = light.ambient * material.ambient;

	// Diffuse intensity
	vec4 diffuseI = light.diffuse * material.diffuse;
	diffuseI = diffuseI * max(dot(normalize(ex_N),normalize(ex_L)),0);

	// Specular intensity
	vec3 R = normalize(reflect(normalize(-ex_L),normalize(ex_N)));
	// not even sure the texture specular is a good thing
	vec4 specularI = light.specular * material.specular; // * vec4(texture(texture_specular[0], TexCoords));
	specularI = specularI * pow(max(dot(R,ex_V),0), material.shininess);

	// Fragment colour
    color = (ambientI + diffuseI + specularI)*vec4(texture(texture_diffuse[0], TexCoords));
}