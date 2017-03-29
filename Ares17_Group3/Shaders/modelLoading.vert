#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 texCoords;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

	
void main()
{
	mat4 BoneTransform = gBones[boneIDs[0]] * weights[0];
    BoneTransform     += gBones[boneIDs[1]] * weights[1];
    BoneTransform     += gBones[boneIDs[2]] * weights[2];
    BoneTransform     += gBones[boneIDs[3]] * weights[3];

	gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(model * (BoneTransform * vec4(position, 1.0)));
    vs_out.Normal = (transpose(inverse(mat4(model))) * (BoneTransform * vec4(normal, 0.0))).xyz;
    vs_out.TexCoords = texCoords;
}