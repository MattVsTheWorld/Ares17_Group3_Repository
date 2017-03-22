// textured.frag
#version 330

// Some drivers require the following
precision highp float;

uniform sampler2D textureUnit0;
uniform vec3 text_color;

in vec2 ex_TexCoord;


layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Fragment colour
	vec4 cols = texture(textureUnit0, ex_TexCoord);
	cols.r -= text_color.r;
	cols.g -= text_color.g;
	cols.b -= text_color.b;
	out_Color = cols;
		// discard texels with alphaof less than 0.5
	if (out_Color.a < 0.5)
		discard;
}