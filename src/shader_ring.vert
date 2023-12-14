#version 330

 
in vec3 a_vertex;
in vec2 a_uv;
in vec3 a_normal;

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_vertex;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4( a_vertex , 1.0 );

    // STEP 2: pass the uv coordinates to the fragment shader
    v_uv = a_uv;
    v_vertex = (u_model * vec4( a_vertex, 1.0 )).xyz; 
    
    mat3 normal_matrix = mat3(transpose(inverse(u_model)));
    v_normal = normal_matrix * a_normal;

    // v_normal = a_normal;

}