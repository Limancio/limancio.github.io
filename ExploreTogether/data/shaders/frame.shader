#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

out vec2 v_uv;

void main() {	
  gl_Position = vec4(position.x, position.y, 0.0f, 1.0);
  v_uv        = uv;
}

#frag
#version 330 core

out vec4 frag_color;

in vec2 v_uv;

uniform sampler2D screen_texture;
uniform float depth_render;

void main() {
  // if(depth_render == 1.0) {
  //   float depth_value = texture(screen_texture, v_uv).r;
  //   frag_color = vec4(vec3(depth_value), 1.0);
  // } 

  frag_color = texture(screen_texture, v_uv);
}
