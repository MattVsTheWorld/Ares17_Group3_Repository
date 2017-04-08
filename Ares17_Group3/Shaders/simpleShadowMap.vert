#version 330 core

// Based on and adapted from the point shadows tutorial by http://learnopengl.com/
// Available: https://learnopengl.com/#!Advanced-Lighting/Shadows/Point-Shadows
// [Accessed: December 2016]

layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 weights;

uniform mat4 model;

const int MAX_BONES = 30;
uniform mat4 gBones[MAX_BONES];


uniform int animated;
void main()
{
	if (animated == 1){
		mat4 BoneTransform = gBones[boneIDs[0]] * weights[0];
		BoneTransform     += gBones[boneIDs[1]] * weights[1];
		BoneTransform     += gBones[boneIDs[2]] * weights[2];
		BoneTransform     += gBones[boneIDs[3]] * weights[3];
		vec4 PosL    = BoneTransform * vec4(position, 1.0f);
		gl_Position = model * PosL;
		}
	else
		gl_Position = model * vec4(position, 1.0);
}  