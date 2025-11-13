#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main() {
  gl_Position = proj_matrix * view_matrix * model_matrix * vec4(position, 1.0);
}

#frag
#version 330 core

out vec4 frag_color;

uniform vec4 u_color = vec4(1.0);

void main() {
  frag_color = u_color;
}
