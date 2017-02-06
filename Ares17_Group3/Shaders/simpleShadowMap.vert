#version 330 core

// Based on and adapted from the point shadows tutorial by http://learnopengl.com/
// Available: https://learnopengl.com/#!Advanced-Lighting/Shadows/Point-Shadows
// [Accessed: December 2016]

layout (location = 0) in vec3 position;
uniform mat4 model;

void main()
{
    gl_Position = model * vec4(position, 1.0);
}  