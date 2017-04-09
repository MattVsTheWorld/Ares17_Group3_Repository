#version 330

uniform mat4 model;
layout (location = 0) in vec3 position;
layout (location = 3) in vec2 texCoords;
out vec2 ex_TexCoord;

//ONLY USED IN HUD (view and projection are ID matrices)

void main(void) {
	gl_Position = model * vec4(position, 1.0f);
	ex_TexCoord = texCoords;
}