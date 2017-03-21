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
	out_Color = texture(textureUnit0, ex_TexCoord);
	// HUD default color is light blue
	out_Color.r -= text_color.r;
	out_Color.g -= text_color.g;
	out_Color.b -= text_color.b;

		// discard texels with alphaof less than 0.5
	if (out_Color.a < 0.5)
		discard;
}