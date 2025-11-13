#version 330 core
layout (location = 0) in vec3  position;
layout (location = 1) in vec2  uv;
layout (location = 2) in float texture;

out vec2  v_uv;
out float v_texture;

uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main() {	
  gl_Position = (proj_matrix * view_matrix) * model_matrix * vec4(position, 1.0);
  v_uv        = uv;
  v_texture   = texture;
}

#frag
#version 330 core

// layout(location = 0) out float fragment_depth;

in vec2  v_uv;
in float v_texture;

uniform sampler2D texture_diffuse;

void main() {
  vec4 pixel_color = texture(texture_diffuse, v_uv);
  if(pixel_color.a < 0.5) {
    discard;
  }
  
  // fragment_depth = gl_FragCoord.z;
}
