#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

out vec2 TexCoords;

uniform mat4 modelMatrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * modelMatrix * vec4(in_Position, 1.0f);
    TexCoords = in_TexCoord;
}