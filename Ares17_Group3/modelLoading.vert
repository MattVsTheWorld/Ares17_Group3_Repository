#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

out vec2 TexCoords;

uniform mat4 modelMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 lightPosition;

out vec3 ex_N;
out vec3 ex_V;
out vec3 ex_L;

void main()
{

	vec4 vertexPosition = view * modelMatrix * vec4(in_Position,1.0);
	ex_V = normalize(-vertexPosition).xyz;
	ex_L = normalize(lightPosition.xyz - vertexPosition.xyz);

	mat3 normalmatrix = transpose(inverse(mat3(view * modelMatrix)));
	ex_N = normalize(normalmatrix * in_Normal);

    
    TexCoords = in_TexCoord;

	gl_Position = projection * view * modelMatrix * vec4(in_Position, 1.0f);
}