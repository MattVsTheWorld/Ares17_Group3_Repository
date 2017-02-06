// cubeMap.vert
// Vertex shader for cubemap for e.g. sky box, with no lights
#version 330

//uniform mat4 modelview;
//uniform mat4 projection;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

smooth out vec3 cubeTexCoord;

layout(location = 0) in vec3 in_Position;

// multiply each vertex position by the modelview projection
void main(void) {
	// vertex into eye coordinates
	vec4 vertexPosition = model * view * vec4(in_Position,1.0);
    gl_Position = projection * vertexPosition;

	cubeTexCoord = normalize(in_Position);
}