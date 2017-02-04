#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec2 in_TexCoord;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform vec4 lightPosition;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;


//out vec3 ex_N;
//out vec3 ex_V;
//out vec3 ex_L;
	
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * normal;
    vs_out.TexCoords = texCoords;

//	vec4 vertexPosition = view * model * vec4(in_Position,1.0);
//	ex_V = normalize(-vertexPosition).xyz;
//	ex_L = normalize(lightPosition.xyz - vertexPosition.xyz);

//	mat3 normalmatrix = transpose(inverse(mat3(view * model)));
//	ex_N = normalize(normalmatrix * in_Normal);
 //   TexCoords = in_TexCoord;
 //	gl_Position = projection * view * model * vec4(in_Position, 1.0f);
}