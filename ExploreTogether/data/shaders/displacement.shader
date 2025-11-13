#version 330 core
layout (location = 0) in vec3  position;
layout (location = 1) in vec3  terrain;
layout (location = 2) in vec2  uv;

out vec3 v_position;
out vec3 v_terrain;
out vec2 v_uv;
// out vec4 v_shadow_coord;

uniform mat4 proj_matrix;
uniform mat4 view_matrix;

// uniform mat4 depth_proj_matrix;
// uniform mat4 depth_view_matrix;
// uniform mat4 depth_model_matrix;

uniform vec4 u_water_plane;

void main() {	
  vec4 world_position = vec4(position, 1.0);
  gl_Position = proj_matrix * view_matrix * world_position; // * vec4(position, 1.0);
  gl_ClipDistance[0] = dot(world_position, u_water_plane);
  
  v_position  = position;
  v_terrain   = terrain;
  v_uv        = uv;
  // v_shadow_coord = (depth_proj_matrix * depth_view_matrix * depth_model_matrix) * vec4(position, 1.0);
}

#frag
#version 330 core

out vec4 frag_color;

in vec2 v_uv;
in vec3 v_terrain;
in vec3 v_position;
// in vec4 v_shadow_coord;

uniform sampler2D texture_bitmap_array[4];

/*
uniform sampler2D depth_bitmap;
float calculate_shadow(vec4 shadow_coord) {
  vec3 proj_coords = shadow_coord.xyz / shadow_coord.w;
  proj_coords = proj_coords * 0.5 + 0.5;
  float closest_depth = texture(depth_bitmap, proj_coords.xy).r; 
  float current_depth = proj_coords.z;
    
  float shadow    = 0.0;
  vec2 texel_size = 1.0 / textureSize(depth_bitmap, 0);
  float bias 	  = 0.005;
  
  for(int x = -1; x <= 1; x++) {
    for(int y = -1; y <= 1; y++) {
      float pcf_depth = texture(depth_bitmap, proj_coords.xy + vec2(x, y) * texel_size).r; 
      shadow += current_depth - bias > pcf_depth ? 0.5 : 0.0;        
    }    
  }
  shadow /= 9.0;
  return shadow;
}
*/

void main() {
  vec3 blend_terrain = v_terrain;
  float main_texture_blend = 1 - (blend_terrain.r + blend_terrain.g + blend_terrain.b);
     
  vec4 main_texture_color = texture(texture_bitmap_array[0], v_uv) * main_texture_blend;
  vec4 r_texture_color    = texture(texture_bitmap_array[1], v_uv) * blend_terrain.r;
  vec4 g_texture_color    = texture(texture_bitmap_array[2], v_uv) * blend_terrain.g;
  vec4 b_texture_color    = texture(texture_bitmap_array[3], v_uv) * blend_terrain.b;
  vec4 final_color = main_texture_color + r_texture_color + g_texture_color + b_texture_color;
     
  frag_color = final_color;
     
  // testing shadow
  // float shadow = calculate_shadow(v_shadow_coord);
  // vec4 light = vec4(1.0 - shadow, 1.0 - shadow, 1.0 - shadow, 1.0);
  // frag_color = light * frag_color;
}
