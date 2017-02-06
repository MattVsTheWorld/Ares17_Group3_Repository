// cubeMap fragment shader
#version 330

// Some drivers require the following
precision highp float;

out vec4 outColour;

smooth in vec3 cubeTexCoord;
uniform samplerCube cubeMap;
 
void main(void) {   
	// Fragment colour
	outColour = texture(cubeMap, cubeTexCoord);
}