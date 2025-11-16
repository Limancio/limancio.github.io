    
uniform sampler2D u_bitmap;
uniform sampler2D u_mask_bitmap;
uniform r32       u_mask_info_array[56];

smooth in vec2 frag_uv;
smooth in vec3 frag_position;
smooth in vec4 frag_color;
smooth in r32  frag_texture_index;

out vec4 out_color;

void main(void) {
    vec4 color = frag_color;
    
    if(frag_texture_index != 0.0f) {
        vec4 sample = texture(u_bitmap, frag_uv);
	float mask_index = texture(u_mask_bitmap, frag_uv).r;
	      
	float grey = 0.21 * sample.r + 0.71 * sample.g + 0.07 * sample.b;        
	float color_factor = u_mask_info_array[0];
	
	if(mask_index > 0) {
	    color_factor = u_mask_info_array[int(mask_index * 255.0)];
	}
	
	color = vec4(sample.r * color_factor + grey * (1.0 - color_factor),
	sample.g * color_factor + grey * (1.0 - color_factor),
	sample.b * color_factor + grey * (1.0 - color_factor), sample.a);

	color = vec4(color.rgb * frag_color.rgb, sample.a * frag_color.a);
    }

    out_color = color;
}
