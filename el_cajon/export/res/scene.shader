#version 330 core
layout (location = 0) in vec2  position;
layout (location = 1) in vec4  color;
layout (location = 2) in vec2  uv;
layout (location = 3) in float texture_id;

out vec4 v_color;
out vec2 v_uv;
out float v_texture_id;

out vec3 v_frag_pos;

uniform mat4 u_proj_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;

void main() {	
	gl_Position  = u_proj_matrix * u_model_matrix * u_view_matrix * vec4(position.x, position.y, 0.0, 1.0);
	v_color      = color;
	v_uv         = uv;
	v_texture_id = texture_id;
    v_frag_pos = vec3((u_model_matrix * vec4(position.x, position.y, 0.0, 1.0)).xyz);
}

#frag
#version 330 core

out vec4 frag_color;

in vec4  v_color;
in vec2  v_uv;
in float v_texture_id;

in vec3  v_frag_pos;  

uniform sampler2D u_bitmap;
uniform vec2 u_mouse_pos;

uniform float u_animate_uv;
uniform vec4  u_uv_offset;

void main() {
	if(v_texture_id == 0) {
  		frag_color = v_color;
  	} else {
  		if(u_animate_uv == 1) {
  			vec2 fixed_uv = v_uv;
  			fixed_uv.x = (u_uv_offset.x * u_uv_offset.z) + v_uv.x * u_uv_offset.z;
  			fixed_uv.y = (u_uv_offset.y * u_uv_offset.w) + v_uv.y * u_uv_offset.w;
  			
	    	frag_color = texture(u_bitmap, fixed_uv); 
  		} else {
  			/*
	    	frag_color = texture(u_bitmap, v_uv); 
			
		  	vec3 light_pos = vec3(u_mouse_pos.x, u_mouse_pos.y, 10.0);
			vec3 light_color = vec3(5.0);
		    float ambient_strength = 0.1;
		    
		    vec3 ambient = ambient_strength * light_color;
		    
		    vec3 norm = vec3(0.0, 0.0, 1.0);
		    vec3 light_dir = normalize(light_pos - v_frag_pos);
		    float diff = max(dot(norm, light_dir), 0.0);
		    vec3 diffuse = diff * light_color;
		
			frag_color = vec4(ambient + diffuse, 1.0) * frag_color;
			*/
			
	    	frag_color = texture(u_bitmap, v_uv); 
  		}
  	}
  	
}