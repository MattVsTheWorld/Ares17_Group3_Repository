// textured.frag
#version 330

// Some drivers require the following
precision highp float;

uniform sampler2D textureUnit0;

in vec2 ex_TexCoord;
layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Fragment colour
	out_Color = texture(textureUnit0, ex_TexCoord);

		// discard texels with alphaof less than 0.5
	if (out_Color.a < 0.5)
		discard;
}