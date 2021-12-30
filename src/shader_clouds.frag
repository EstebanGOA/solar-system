#version 330

out vec4 fragColor;
in vec2 v_uv;

uniform sampler2D u_texture;
// uniform sampler2D u_texture_transparency;
uniform float u_alpha;

void main(void)
{
	
	vec4 texture_color = texture(u_texture, v_uv);
	// vec4 texture_transparency = texture(u_texture_transparency, v_uv);

	// Consideramos que la parte transparente de la textura es el color azul, esto porque estamos usando BMP y no tenemos canal alpha.
	//  texture_color.x >= 0.0 && texture_color.x <= 0.01 && texture_color.y >= 0.01 && texture_color.y <= 0.06 && texture_color.z >= 0.5 && texture_color.z <= 0.8 
	if ( texture_color.x <= 0.3 && texture_color.y <= 0.3 && texture_color.z <= 0.3 ) discard;
	else fragColor = vec4(texture_color.xyz, u_alpha);
}