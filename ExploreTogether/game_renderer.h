#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

struct game_renderer {
  renderer_draw_data frame_render_data;
  
  renderer_buffer_data scene_buffer_data;
  renderer_buffer_data editor_buffer_data;
  
  renderer_buffer_data reflect_buffer_data;
  renderer_buffer_data refract_buffer_data;
  renderer_buffer_data refract_depth_buffer_data;
  
  shader_program* scene_shader;
  shader_program* frame_shader;
  shader_program* depth_shader;
  shader_program* water_shader;
  shader_program* displacement_shader;
  
  // @note: Utility cached matrices.
  // mat4 ortho_matrix;
  // mat4 persp_matrix;
  // mat4 view_matrix;
};

internal void
renderer_draw_entity(catalog_handler* catalog_handler, shader_program* draw_shader, entity* entity);

#endif // !SCENE_RENDERER_H
