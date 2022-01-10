#version 330

out vec4 fragColor;
in vec2 v_uv;

uniform sampler2D u_texture;
uniform float u_alpha;

void main(void)
{
	
	vec4 texture_color = texture(u_texture, v_uv);

	// Consideramos que la parte transparente de la textura es el color negro, esto porque estamos usando BMP y no tenemos canal alpha.
	if ( texture_color.x >= 0.9 && texture_color.y >= 0.9 && texture_color.z >= 0.9 ) discard;
	else fragColor = vec4(texture_color.xyz, 1.0);
}