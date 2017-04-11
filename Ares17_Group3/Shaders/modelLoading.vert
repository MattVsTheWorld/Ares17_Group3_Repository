#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 texCoords;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 50;
uniform mat4 gBones[MAX_BONES];

uniform int animated;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

	
void main()
{
	if (animated == 1){
		mat4 BoneTransform = gBones[boneIDs[0]] * weights[0];
		BoneTransform     += gBones[boneIDs[1]] * weights[1];
		BoneTransform     += gBones[boneIDs[2]] * weights[2];
		BoneTransform     += gBones[boneIDs[3]] * weights[3];

		vec4 PosL    = BoneTransform * vec4(position, 1.0f);
		gl_Position = projection * view * model * PosL;
	}
	else {
		gl_Position = projection * view * model * vec4(position, 1.0f);
		//vs_out.FragPos = vec3(model * vec4(position, 1.0));
		//vs_out.Normal = transpose(inverse(mat3(model))) * normal;
	}

	vs_out.FragPos = vec3(model * vec4(position, 1.0));
	vs_out.Normal = transpose(inverse(mat3(model))) * normal;
	vs_out.TexCoords = texCoords;
}