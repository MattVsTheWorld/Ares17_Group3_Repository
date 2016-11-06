// textured.vert
// use textures, but no lighting
#version 330

uniform mat4 modelview;
uniform mat4 projection;

in  vec3 in_Position;
//in  vec3 in_Colour; // colour not used with lighting
//in  vec3 in_Normal;
//out vec4 ex_Color;

in vec2 in_TexCoord;
out vec2 ex_TexCoord;

// multiply each vertex position by the MVP matrix
void main(void) {

	// vertex into eye coordinates
	vec4 vertexPosition = modelview * vec4(in_Position,1.0);
    gl_Position = projection * vertexPosition;

	ex_TexCoord = in_TexCoord;
}