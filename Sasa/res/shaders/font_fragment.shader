    
uniform sampler2D u_bitmap;

smooth in vec2 frag_uv;
smooth in vec3 frag_position;
smooth in vec4 frag_color;
smooth in r32  frag_texture_index;

out vec4 out_color;

void main(void) {
    vec4 color = frag_color;
    color = color * texture(u_bitmap, frag_uv).r;
    
    out_color = color;
}
