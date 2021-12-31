#version 330

out vec4 fragColor;

// STEP 3: get the uv coordinates form the vertex shader
in vec2 v_uv;
in vec3 v_normal; 
in vec3 v_vertex;

// Values to assign textures
uniform sampler2D u_texture;

// Values to assign night textures;
uniform sampler2D u_texture_night;

// Values to set alpha on different planets
uniform float u_alpha;
uniform float u_clouds;

// Variables to represet light
uniform vec3 u_light_dir;
uniform vec3 u_cam_pos;
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;

// normal
uniform sampler2D u_texture_normal;

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
    vec3 texture_normal = texture(u_texture_normal, v_uv).xyz;
    vec3 N = normalize(v_normal);
	vec3 N_orig = N;
	N = perturbNormal(N, v_vertex, v_uv, texture_normal);
	N = mix(N_orig, N, 1.0f);
    
	vec3 L = normalize(u_light_dir - v_vertex);
    vec3 R = normalize(-reflect(L, N));
    vec3 E = normalize(u_cam_pos - v_vertex);
    
	float NdotL = max(dot(N, L), 0.0);
    float RdotE = max(dot(R, E), 0.0);

    vec4 texture_color = texture(u_texture, v_uv);
	vec4 texture_night = texture(u_texture_night, v_uv);

    vec3 final_color = texture_color.xyz * u_ambient + texture_color.xyz * u_diffuse * NdotL + texture_color.xyz * u_specular * pow(RdotE, u_shininess);
    
	// Si u_clouds es 1.0 estamos dibujando las nubes del planeta, por tanto, descartamos las partes de la textura que tengan un color cercano al negro.
	if ( u_clouds == 0.0 ) {
		if ( final_color.x <= 0.09 && final_color.y <= 0.09 && final_color.z <= 0.09 ) {
			fragColor = vec4(texture_night.xyz, u_alpha);
		} else {
			fragColor = vec4(final_color, u_alpha);
		}
	} else {
		if ( texture_color.x <= 0.3 && texture_color.y <= 0.3 && texture_color.z <= 0.3 ) discard;
		// Modificando el u_alpha podemos hacer que la textura sea más o menos transparente. 
		fragColor =  vec4(final_color, u_alpha);
	}

}
