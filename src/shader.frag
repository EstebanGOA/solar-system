#version 330

out vec4 fragColor;

// STEP 3: get the uv coordinates form the vertex shader
in vec2 v_uv;
in vec3 v_normal; 
in vec3 v_vertex;

uniform sampler2D u_texture;

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

    vec3 N = normalize(v_normal);
	vec3 L = normalize(u_light_dir - v_vertex);
    vec3 R = normalize(-reflect(L, N));
    vec3 E = normalize(u_cam_pos - v_vertex);
    
	float NdotL = max(dot(N, L), 0.0);
    float RdotE = max(dot(R, E), 0.0);

	/*	
	float spec = 0.50f;
	vec3 halfwayDir = normalize(E + L);


	float spec = pow(max(dot(normal, halfwayDir), 0.0), u_shininess);
	vec3 specular = lightColor * spec;

	*/

    vec4 texture_color = texture(u_texture, v_uv);

    vec3 final_color = texture_color.xyz * u_ambient + texture_color.xyz * u_diffuse * NdotL + texture_color.xyz * u_specular * pow(RdotE, u_shininess);
    fragColor =  vec4(final_color, 1.0);
    
}
