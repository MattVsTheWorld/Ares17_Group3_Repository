// textured.vert
// use textures, but no lighting
#version 330

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 texCoords;
//in  vec3 in_Colour; // colour not used with lighting
//in  vec3 in_Normal;
//out vec4 ex_Color;

out vec2 ex_TexCoord;

// multiply each vertex position by the MVP matrix
void main(void) {

	// vertex into eye coordinates
	gl_Position = projection * view * model * vec4(position, 1.0f);
	ex_TexCoord = texCoords;
}