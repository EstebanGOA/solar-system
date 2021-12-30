#version 330

out vec4 fragColor;

in vec2 v_uv;
uniform vec3 u_color;

in vec3 v_normal;
in vec3 v_vertex;

uniform sampler2D u_texture;
uniform sampler2D u_texture_normal;

uniform vec3 u_light_dir;
uniform vec3 u_cam_pos;

uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;


mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p );
	vec2 duv1 = dFdx( uv );
	vec2 duv2 = dFdy( uv );
	// solve the linear system
	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 );
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
			 
	// construct a scale-invariant frame 
	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
	return mat3( T * invmax, B * invmax, N );
}

// assume N, the interpolated vertex normal and 
// V, the view vector (vertex to eye)
vec3 perturbNormal( vec3 N, vec3 V, vec2 texcoord, vec3 normal_pixel )
{

	normal_pixel = normal_pixel * 2.0 - 1.0;
	mat3 TBN = cotangent_frame(N, V, texcoord);
	return normalize(TBN * normal_pixel);
}


void main(void)
{
    
    vec4 texture_color = texture(u_texture, v_uv);

	
	vec3 N = normalize(v_normal);

    vec3 L = normalize(u_light_dir - v_vertex);


    
    vec3 R = normalize(-reflect(L,N) );
    vec3 E = normalize(u_cam_pos - v_vertex);

    vec3 final_color = texture_color.xyz * u_ambient + texture_color.xyz * u_diffuse * dot(N, L) + texture_color.xyz * u_specular * pow ( float(dot(R, E)), float(u_shininess));
    fragColor = vec4( final_color, 1.0);
    
   
    
}
