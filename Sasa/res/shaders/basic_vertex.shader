in vec2 vertex_point;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec4 vertex_color;
in r32  vertex_texture_index;

smooth out vec2 frag_uv;
smooth out vec3 frag_position;
smooth out vec4 frag_color;
smooth out r32  frag_texture_index;

uniform mat4 u_proj_matrix;
uniform mat4 u_view_matrix;

void main(void) {
    gl_Position = u_proj_matrix * u_view_matrix * vec4(vertex_point, 0.0f, 1.0f);

    frag_position = vec3(vertex_point, 0);
    frag_uv     = vertex_uv;
    frag_color  = vertex_color;
    frag_texture_index = vertex_texture_index;
}
