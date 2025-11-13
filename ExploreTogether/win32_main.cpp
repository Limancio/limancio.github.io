#include <windows.h>
#include <stdio.h> 

#include "window/game_window.h"

#include "constant.h"
#include "graphics/gui.cpp"
#include "graphics/renderer.cpp"

#include "graphics/shader_program.cpp"
#include "graphics/mesh_catalog.cpp"
#include "graphics/game_camera.cpp"

#include "game.cpp"
#include "game_editor.cpp"
#include "game_renderer.cpp"

int main(int argc, char* argv[]){
  string_bucket* string_list = NULL;
  
  win32_window win32_window = {};
  init_window(&win32_window, string_list, "game_window", 800, 600);
  
  game_window game_window = {};
  game_window.width   = (r32) win32_window.width;
  game_window.height  = (r32) win32_window.height;
  game_window.iwidth  = (s32) win32_window.width;
  game_window.iheight = (s32) win32_window.height;
  
  catalog_handler  catalog_handler  = {};
  shader_catalog   shader_catalog   = {};
  texture_catalog  texture_catalog  = {};
  mesh_catalog     mesh_catalog     = {};
  material_catalog material_catalog = {};
  init_catalog_handler(&catalog_handler, &shader_catalog, &texture_catalog, &mesh_catalog, &material_catalog);
  
  ui_handler    ui_handler    = {};
  game_state    game_state    = {};
  game_editor   game_editor   = {};
  game_renderer game_renderer = {};
  
  init_ui(&ui_handler, &game_window, &catalog_handler, string_list, const_string("data/shaders/ui.shader"), const_string("data/fonts/georgia.ttf"));
  init_game_renderer(&game_renderer, &game_window, string_list, &shader_catalog);
  init_game_editor(&game_editor, &game_renderer, &game_state, &game_window, string_list, &catalog_handler);
  
  { // @temporary: move to init_level
    // load_game_state(&game_state, &renderer, &mesh_catalog, &texture_mesh_catalog);
    game_level* level = &game_state.level;
    string_alloc(string_list, level->level_name, const_string("level_name"));
    
    mesh_file_data sky_mesh_data = mesh_catalog_add(&catalog_handler, const_string("data/models/skybox/sky_dome.dae"));
    renderer_submit_mesh(&sky_mesh_data.mesh_data_array[0], &level->sky_mesh);
    
    game_editor_add_game_object(&game_editor, &game_window, STATIC_OBJECT);
    
    game_camera* camera = &level->camera;
    mat_perspective(game_state.persp_matrix, camera->fov, game_window.width / game_window.height, 0.1f, 600.0f);
    mat_ortho(game_state.ortho_matrix, 0.0f, game_window.height, 0.0f, game_window.width, -10.0f, 10.0f);
    mat_look_at(game_state.view_matrix, camera->position, camera->target, camera->up);
  }  
  
  r64 delta_time = 0.0f;
  r64 last_frame = 0.0f;
  while(!close_window(&win32_window) && !game_state.exit) {
    r64 curr_frame = glfwGetTime();
    delta_time = curr_frame - last_frame;
    last_frame = curr_frame;

    game_editor.select_elapsed += delta_time;
    if(game_editor.select_elapsed > 3.0) {
      game_editor.select_elapsed -= 3.0;
    }
    
    init_game_window(&win32_window, &game_window);
    if(game_window.resized) {
      game_window.resized = false;
      game_camera* camera = &game_state.level.camera;
      
      mat_perspective(game_state.persp_matrix, camera->fov, game_window.width / game_window.height, 0.1f, 600.0f);
      mat_ortho(game_state.ortho_matrix, 0.0f, game_window.height, 0.0f, game_window.width, -10.0f, 10.0f);
      
      update_renderer_buffer_data(&game_renderer.scene_buffer_data, game_window.iwidth, game_window.iheight);
      update_renderer_buffer_data(&game_renderer.editor_buffer_data, game_window.iwidth, game_window.iheight);

      update_renderer_buffer_data(&game_renderer.reflect_buffer_data, game_window.iwidth, game_window.iheight);
      update_renderer_buffer_data(&game_renderer.refract_buffer_data, game_window.iwidth, game_window.iheight);
      update_renderer_depth_buffer_data(&game_renderer.refract_depth_buffer_data, game_window.iwidth, game_window.iheight);
      
      update_renderer_buffer_data(&game_editor.mesh_viewer_buffers, game_window.iwidth, game_window.iheight);
      update_renderer_buffer_data(&game_editor.editor_buffers, game_window.iwidth, game_window.iheight);
      ui_resize(&ui_handler, &game_window);
    }
    
    if(game_window.typed_keys[GLFW_KEY_F1]) {
      game_editor.show_editor = !game_editor.show_editor;
      if(game_editor.show_editor) {
	// load game editor
      } else {
	// load level
      }
    }
    
    if(!game_editor.show_editor) { // update game state
      game_state_update(&game_state, &game_window, (r32) delta_time);
    }
    
    {
      game_camera* camera = &game_state.level.camera;
      update_camera_view_matrix(game_state.view_matrix, camera);
    }
    
#if 0
    { // @temporary: anim mesh update:
      mesh_arena* arena = &mesh_catalog.mesh_arena;
      mesh_dynamic_bucket* bucket = arena->bucket_dynamic_list;

      while(bucket != NULL) {
	for(u32 index = 0; index < MESH_BUCKET_COUNT; index++) {
	  mesh_dynamic_data* it = &bucket->mesh_data_array[index];
	  
	  if(it->usage_count) {
	    animation_update(&it->mesh, delta_time);
	  }
	}
	bucket = bucket->next_bucket;
      }
    }
#endif
    
    if(game_editor.show_editor) {
      ui_renderer* ui_renderer = &ui_handler.renderer;
      shader_set_framebuffer(ui_renderer->buffer_data.frame_buffer);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
      shader_program* ui_shader = ui_renderer->shader;
      glUseProgram(ui_shader->id);
      shader_set_matrix(&shader_catalog, ui_shader, const_string("proj_matrix"), game_state.ortho_matrix);
      game_editor_ui(&game_editor, &game_state, &game_window, &ui_handler, (r32) delta_time);
      
      glUseProgram(0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    {
      // @temporary: mesh visualizer drawing
      draw_mesh_viewer(&game_editor, (r32) delta_time);
      
      // draw water to buffers
      if(game_state.level.ocean_data.active) {
	draw_water_into_buffers(&game_state, &game_editor, &catalog_handler, &game_renderer);
      }
      
      // draw scene to FBO
      shader_set_framebuffer(game_renderer.scene_buffer_data.frame_buffer);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      draw_level_world(&game_state, &game_editor, &catalog_handler, &game_renderer, game_state.view_matrix);
      if(game_state.level.ocean_data.active) {
	draw_level_water(&game_state, &catalog_handler, &game_renderer);
      }
      
      shader_set_framebuffer(0);
    }
    
    //
    // game editor: @temporary: needs to be callead after render the scene
    //
    
    if(game_editor.show_editor) {
      game_editor_update(&game_editor, &game_state, &game_window, &ui_handler, (r32) delta_time);
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    shader_active(game_renderer.frame_shader);
    glBindVertexArray(game_renderer.frame_render_data.vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, game_renderer.scene_buffer_data.texture_buffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    if(game_editor.show_editor) {
      ui_renderer* ui_renderer = &ui_handler.renderer;
      
      glBindTexture(GL_TEXTURE_2D, ui_renderer->buffer_data.texture_buffer);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    glBindVertexArray(0);  
    glEnable(GL_DEPTH_TEST);
    shader_active(0);   
    
    update_game_window(&win32_window, &game_window);
  }
  // save_game_state(&game_state);
  
  end_window();
  return(0);
}
  
#if 0
  {
    // @cleanup
    mesh* campfire = mesh_catalog_get(&mesh_catalog, "campfire");
    mesh_data* data = &campfire->mesh_data;

    material* material = &data->material_array[0];
    texture* animation = material->texture;

    material->uv_animation = true;
    material->tile_map_frame_size  = { 128.0f, 128.0f };
    material->tile_map_total_size  = { (r32) animation->width, (r32) animation->height };
    material->tile_map_frame_count = { material->tile_map_total_size.x / material->tile_map_frame_size.x, material->tile_map_total_size.y / material->tile_map_frame_size.y };
    material->tile_map_uv_size 	   = { (material->tile_map_frame_size.x / material->tile_map_total_size.x), (material->tile_map_frame_size.y / material->tile_map_total_size.y) };

    material->frame_count = material->tile_map_frame_count.x * material->tile_map_frame_count.y;
    material->frame_time  = 0.05f;
    material->total_time  = material->frame_time * material->frame_count;

    for(u32 i = 0; i < data->material_id_count; i++) {
      if(data->material_id_array[i] == 0) {
	data->uv_array[i*2]   = data->uv_array[ i*2 ] * material->tile_map_uv_size.x;
	data->uv_array[i*2+1] = data->uv_array[i*2+1] * material->tile_map_uv_size.y;
      }
    }
    
    r32 offset = 0;
    game_state.level.game_object_count = mesh_catalog.pool_count;
    for(u32 i = 0; i < game_state.level.game_object_count; i++) {
      entity* game_object = &game_state.level.game_object_array[i];
      game_object->mesh = &mesh_catalog.pool[i];
      game_object->position.x = offset;
      update_model_matrix(game_object);
      renderer_submit_mesh(&renderer, game_object->mesh);
      offset += 1.5f;
    }
  }

// @cleanup: uv animated textures stuff
    for(u32 i = 0; i < mesh_catalog.pool_count; i++) {
      mesh_data* data = &mesh_catalog.pool[i].mesh_data;
      for(u32 i = 0; i < data->material_count; i++) {
	material* material = &data->material_array[i];
	
	if(material->uv_animation) {
	  material->elapsed_time += delta_time;
	  material->current_frame = floor(material->elapsed_time / material->frame_time);
	  if(material->current_frame > material->frame_count) {
	    material->current_frame = 0;
	    material->elapsed_time = 0.0f;
	  }
	  
	  u32 offset_frame_y = floor(material->current_frame / material->tile_map_frame_count.x);
	  u32 offset_frame_x = material->current_frame - (offset_frame_y * material->tile_map_frame_count.x);
	  material->uv_offset.x = material->tile_map_uv_size.x * offset_frame_x; 
	  material->uv_offset.y = material->tile_map_uv_size.y * offset_frame_y; 
	}
      }
    }
#endif
