#include "game_renderer.h"

internal void
init_game_renderer(game_renderer* renderer, game_window* window, string_bucket* string_list, shader_catalog* shader_catalog) {
  renderer_frame_buffer_data_init(&renderer->frame_render_data);
  
  renderer->scene_shader = shader_catalog_add(shader_catalog, string_list, const_string("data/shaders/scene.shader"), const_string("scene"));
  renderer->frame_shader = shader_catalog_add(shader_catalog, string_list, const_string("data/shaders/frame.shader"), const_string("frame"));
  renderer->water_shader = shader_catalog_add(shader_catalog, string_list, const_string("data/shaders/water.shader"), const_string("water"));
  renderer->depth_shader = shader_catalog_add(shader_catalog, string_list, const_string("data/shaders/depth.shader"), const_string("depth"));
  renderer->displacement_shader = shader_catalog_add(shader_catalog, string_list, const_string("data/shaders/displacement.shader"), const_string("displacement"));
  
  shader_active(renderer->frame_shader);
  shader_set_valuei(shader_catalog, renderer->frame_shader, const_string("u_screen_bitmap"), 0);
  shader_active(0);
  
  shader_active(renderer->scene_shader);
  for(u32 i = 0; i < MAX_TEXTURE_PER_MESH; i++) {
    s8 texture_name[32];
    stbsp_sprintf(texture_name, "u_texture_array[%i]", i);
    string texture_uniform = stack_string(texture_name);
    
    glActiveTexture(GL_TEXTURE0 + 1);
    shader_set_valuei(shader_catalog, renderer->scene_shader, texture_uniform, 0);
  }  
  shader_active(0);
  
  shader_active(renderer->depth_shader);
  glActiveTexture(GL_TEXTURE0);
  shader_set_valuei(shader_catalog, renderer->depth_shader, const_string("depth_bitmap"), 0);
  shader_active(0);
  
  shader_active(renderer->water_shader);
  glActiveTexture(GL_TEXTURE0);
  shader_set_valuei(shader_catalog, renderer->water_shader, const_string("u_reflect_bitmap"), 0);
  glActiveTexture(GL_TEXTURE1);
  shader_set_valuei(shader_catalog, renderer->water_shader, const_string("u_refract_bitmap"), 1);
  glActiveTexture(GL_TEXTURE2);
  shader_set_valuei(shader_catalog, renderer->water_shader, const_string("u_dudv_bitmap"), 2);
  glActiveTexture(GL_TEXTURE3);
  shader_set_valuei(shader_catalog, renderer->water_shader, const_string("u_depth_bitmap"), 3);
  shader_active(0);
  
  shader_active(renderer->displacement_shader);
  glActiveTexture(GL_TEXTURE0);
  shader_set_valuei(shader_catalog, renderer->displacement_shader, const_string("u_texture_bitmap_array[0]"), 0);
  glActiveTexture(GL_TEXTURE1);
  shader_set_valuei(shader_catalog, renderer->displacement_shader, const_string("u_texture_bitmap_array[1]"), 1);
  glActiveTexture(GL_TEXTURE2);
  shader_set_valuei(shader_catalog, renderer->displacement_shader, const_string("u_texture_bitmap_array[2]"), 2);
  glActiveTexture(GL_TEXTURE3);
  shader_set_valuei(shader_catalog, renderer->displacement_shader, const_string("u_texture_bitmap_array[3]"), 3);
  glActiveTexture(GL_TEXTURE4);
  shader_set_valuei(shader_catalog, renderer->displacement_shader, const_string("u_depth_bitmap"), 4);    
  shader_active(0);
  
  init_renderer_buffer_data(&renderer->scene_buffer_data, window->iwidth, window->iheight);
  
  init_renderer_buffer_data(&renderer->reflect_buffer_data, window->iwidth, window->iheight);
  init_renderer_buffer_data(&renderer->refract_buffer_data, window->iwidth, window->iheight);
  init_renderer_depth_buffer_data(&renderer->refract_depth_buffer_data, window->iwidth, window->iheight);
  
#if 0
  // frame_buffer_prepare(renderer->editor_render_buffer,renderer->editor_frame_buffer, renderer->editor_texture, window_width, window_height);
  // init_renderer_buffer_data(renderer->mesh_viewer_render_buffer,   renderer->mesh_viewer_frame_buffer,    renderer->mesh_viewer_texture,    window_width, window_height);
  
  //gizmo buffer prepare
  glGenVertexArrays(1, &renderer->gizmo_vao);
  glGenBuffers(1, &renderer->gizmo_vbo);
  
  glBindVertexArray(renderer->gizmo_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, renderer->gizmo_vbo);
  lglBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 6 * 25, 0, GL_DYNAMIC_DRAW); // @cleanup CHANGE IF BIG MESH IS RENDERER :D
  glEnableVertexAttribArray(SHADER_VERTEX_INDEX);
  glVertexAttribPointer(SHADER_VERTEX_INDEX,   3, GL_FLOAT, GL_FALSE, sizeof(vec3), (const GLvoid*) offsetof(vertex, position));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  glBindVertexArray(0);
#endif
}

internal void
renderer_draw_entity(catalog_handler* catalog_handler, shader_program* draw_shader, entity* entity) {
  shader_catalog* shader_catalog = catalog_handler->shader_catalog;
  shader_set_matrix(shader_catalog, draw_shader, const_string("model_matrix"), entity->model_matrix);
  
  renderer_draw_mesh(catalog_handler, draw_shader, &entity->mesh_render);
}

internal void
renderer_draw_displacement_entity(catalog_handler* catalog_handler, shader_program* draw_shader, displacement* displacement) {
  renderer_draw_displacement(catalog_handler, draw_shader, &displacement->mesh_render);
}

internal void
draw_level_world(game_state* state, game_editor* editor, catalog_handler* catalog_handler, game_renderer* renderer, r32* camera_view_matrix) { // view_matrix looks redundants but is needed for renderer the scene inverted for reflections
  shader_catalog* shader_catalog = catalog_handler->shader_catalog;
  game_level* level   = &state->level;
  game_camera* camera = &level->camera;

  mat4 mat;
  imat4(mat);
  shader_active(renderer->displacement_shader);
  shader_set_matrix(shader_catalog, renderer->displacement_shader, const_string("proj_matrix"), state->persp_matrix);
  shader_set_matrix(shader_catalog, renderer->displacement_shader, const_string("view_matrix"), state->view_matrix);
  
  size_t displacement_count = hmlen(level->displacement_array);
  for(u32 i = 0; i < displacement_count; i++) {
    displacement* displacement = &level->displacement_array[i].value;
    renderer_draw_displacement_entity(catalog_handler, renderer->displacement_shader, displacement);   
  }
  shader_active(0);
  
  vec3 select_color = {1.0f, 1.0f, 0.0f};
  shader_active(renderer->scene_shader);
  
  shader_set_matrix(shader_catalog, renderer->scene_shader, const_string("proj_matrix"), state->persp_matrix);
  shader_set_matrix(shader_catalog, renderer->scene_shader, const_string("view_matrix"), camera_view_matrix);
  shader_set_value( shader_catalog, renderer->scene_shader, const_string("select_color"), select_color);
  shader_set_valuef(shader_catalog, renderer->scene_shader, const_string("select_elapsed"), (r32) editor->select_elapsed);
  
  size_t entity_count = hmlen(level->entity_array);
  for(u32 i = 0; i < entity_count; i++) {
    entity* it = &level->entity_array[i].value; 
    b32 is_select = game_editor_is_entity_select(editor, it->id);
    
    shader_set_valuef(shader_catalog, renderer->scene_shader, const_string("is_select"), (is_select) ? 1.0f : 0.0f);
    renderer_draw_entity(catalog_handler, renderer->scene_shader, it);
  } 
  shader_active(0);
  
#if 0
  for(u32 i = 0; i < level->static_object_count; i++) {
    static_object* game_object = &level->static_object_array[i];
    entity* entity = get_entity_by_id(level, game_object->entity_id);
    
    renderer_draw_entity(catalog_handler, renderer->scene_shader, entity);
  }
  
  for(u32 i = 0; i < level->dynamic_object_count; i++) {
    dynamic_object* game_object = &level->dynamic_object_array[i];
    entity* entity = get_entity_by_id(level, game_object->entity_id);
    
    renderer_draw_entity(catalog_handler, renderer->scene_shader, entity);
  }

  if(level->player_on_level) {
    player* player = &level->player;
    entity* entity = get_entity_by_id(level, player->entity_id);
    
    renderer_draw_entity(catalog_handler, renderer->scene_shader, entity);
  }
#endif
}

internal void
draw_level_water(game_state* state, catalog_handler* catalog_handler, game_renderer* renderer) {
  shader_catalog* shader_catalog = catalog_handler->shader_catalog;
  game_level* level   = &state->level;
  game_camera* camera = &level->camera;
  
  shader_active(renderer->water_shader);
  shader_set_matrix(shader_catalog, renderer->water_shader, const_string("proj_matrix"), state->persp_matrix);
  shader_set_matrix(shader_catalog, renderer->water_shader, const_string("view_matrix"), state->view_matrix);
  
  shader_set_value(shader_catalog, renderer->water_shader, const_string("u_camera_position"), camera->position);
  
  mat4 model_matrix;
  imat4(model_matrix);
  shader_set_matrix(shader_catalog, renderer->water_shader, const_string("model_matrix"), model_matrix);
  
  ocean& ocean = level->ocean_data;
  shader_set_value(shader_catalog, renderer->water_shader, const_string("u_ocean.water_color"), ocean.water_color);
  shader_set_valuef(shader_catalog, renderer->water_shader, const_string("u_ocean.clearness"), ocean.clearness);
  shader_set_valuef(shader_catalog, renderer->water_shader, const_string("u_ocean.border_alpha_power"), ocean.border_alpha_power);
  shader_set_valuef(shader_catalog, renderer->water_shader, const_string("u_ocean.tile_bias"), ocean.tile_bias);
  shader_set_valuef(shader_catalog, renderer->water_shader, const_string("u_ocean.wave_strength"), ocean.wave_strength);
  shader_set_valuef(shader_catalog, renderer->water_shader, const_string("u_ocean.refractivity"), ocean.refractivity);

  shader_set_valuef(shader_catalog, renderer->water_shader, const_string("u_wave_length"), ocean.wave_length);
  shader_set_valuef(shader_catalog, renderer->water_shader, const_string("u_wave_amplitude"), ocean.wave_amplitude);
  shader_set_valuef(shader_catalog, renderer->water_shader, const_string("u_move_elapsed"), ocean.water_move_elapsed);
  
  // @temporary:
  shader_set_value(shader_catalog, renderer->water_shader, const_string("u_ocean.wave1_force_direction"), ocean.wave1_force_direction);
  shader_set_value(shader_catalog, renderer->water_shader, const_string("u_ocean.wave2_force_direction"), ocean.wave2_force_direction);
  
  // @cleanup
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer->reflect_buffer_data.texture_buffer);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, renderer->refract_buffer_data.texture_buffer);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, ocean.dudv_texture->id);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, renderer->refract_depth_buffer_data.texture_buffer);
  
  renderer_draw_mesh(catalog_handler, renderer->scene_shader, &level->ocean_mesh);
  glUseProgram(0);
}

internal void
draw_water_into_buffers(game_state* state, game_editor* editor, catalog_handler* catalog_handler, game_renderer* renderer) {
  shader_catalog* shader_catalog = catalog_handler->shader_catalog;
  game_level* level   = &state->level;
  game_camera* camera = &level->camera;
  
  ocean& ocean = level->ocean_data;
  // draw reflect water to FBO
  shader_enable(GL_CLIP_DISTANCE0);
  {
    renderer_buffer_data* reflect_buffers = &renderer->reflect_buffer_data;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_set_framebuffer(reflect_buffers->frame_buffer);
    
    vec4 water_plane = {0, 1, 0, -1};
    shader_active(renderer->scene_shader);
    shader_set_value(shader_catalog, renderer->scene_shader, const_string("u_water_plane"), water_plane);
    shader_active(0);
    
    shader_active(renderer->displacement_shader);
    shader_set_value(shader_catalog, renderer->displacement_shader, const_string("u_water_plane"), water_plane);
    shader_active(0);
    
    mat4 inverted_view_matrix;
    r32 camera_pitch  = camera->pitch;
    r32 camera_offset = 2.0f * (camera->position.y - ocean.water_height);

    // @cleanup
    camera->position.y -= camera_offset;
    camera->pitch 	= -camera_pitch; 
    update_camera_view_matrix(inverted_view_matrix, camera);
    camera->position.y += camera_offset;
    camera->pitch 	= camera_pitch; 
    update_camera_view_matrix(state->view_matrix, camera);

    // draw sky dome
    shader_active(renderer->scene_shader);
    shader_set_matrix(shader_catalog, renderer->scene_shader, const_string("proj_matrix"), state->persp_matrix);
    
    mat4 sky_view_matrix;
    memcpy(sky_view_matrix, inverted_view_matrix, sizeof(mat4));
    sky_view_matrix[12] = 0.0f;
    sky_view_matrix[13] = 0.0f;
    sky_view_matrix[14] = 0.0f;
    shader_set_matrix(shader_catalog, renderer->scene_shader, const_string("view_matrix"), sky_view_matrix);
    
    mat4 model_matrix;
    imat4(model_matrix);
    shader_set_matrix(shader_catalog, renderer->scene_shader, const_string("model_matrix"), model_matrix);
    renderer_draw_mesh(catalog_handler, renderer->scene_shader, &level->sky_mesh);
    shader_active(0);
    
    draw_level_world(state, editor, catalog_handler, renderer, inverted_view_matrix);
      
    shader_set_framebuffer(0);
  }
  
  {
    // draw refract depth to FBO
    vec4 water_plane = {0, -1, 0, 1};
    shader_active(renderer->scene_shader);
    shader_set_value(shader_catalog, renderer->scene_shader, const_string("u_water_plane"), water_plane);
    shader_active(0);
    shader_active(renderer->displacement_shader);
    shader_set_value(shader_catalog, renderer->scene_shader, const_string("u_water_plane"), water_plane);
    shader_active(0);
    
    renderer_buffer_data* refract_depth_buffers = &renderer->refract_depth_buffer_data;
    shader_set_framebuffer(refract_depth_buffers->frame_buffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    draw_level_world(state, editor, catalog_handler, renderer, state->view_matrix);
    shader_set_framebuffer(0);
    
    // draw refract water to FBO
    renderer_buffer_data* refract_buffers = &renderer->refract_buffer_data;
    shader_set_framebuffer(refract_buffers->frame_buffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    draw_level_world(state, editor, catalog_handler, renderer, state->view_matrix);
    shader_set_framebuffer(0);
  }
  shader_disable(GL_CLIP_DISTANCE0);
}
