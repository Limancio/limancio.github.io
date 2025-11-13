#ifndef EDITOR_PANEL_H
#define EDITOR_PANEL_H

#include "graphics/gui.cpp"

internal void
list_tool_panel(game_editor* game_editor, game_state* state, game_window* window, renderer* renderer, mesh_catalog* mesh_catalog, texture_catalog* texture_catalog) {
  ui_handler* ui = &game_editor->ui_handler;

  r32 w = ui->window_layout.x;
  r32 h = ui->window_layout.y;
  rect initial_rect = {0, h, w * 0.15f, h * 0.45f};
  
  string panel_title = const_string("Editor");
  region_state* region_state = region_start(ui, UI_ID(0), panel_title, initial_rect, REGION_TITLE);
  {
    basic_style override_style = ui->default_basic_style;
    override_style.active_flags = FILL_LABEL;
    
    update_div_count(region_state, 2);
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("entity"), CLASSIC_BUTTON, &override_style)) {
      game_editor->tool = editor_tool::EDIT_ENTITY;
    }
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("ocean"), CLASSIC_BUTTON, &override_style)) {
      game_editor->tool = editor_tool::EDIT_OCEAN;
    }
    
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("displacement"), CLASSIC_BUTTON, &override_style)) {
      game_editor->tool = editor_tool::EDIT_DISPLACEMENT;
    }
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("mesh"), CLASSIC_BUTTON, &override_style)) {
      game_editor->tool = editor_tool::MESH_INSTANCE_PANEL;
    }
    
    update_div_count(region_state, 0);
  }
  region_end(ui);

  recovery_panel(game_editor, state, window, renderer, mesh_catalog, texture_catalog);  
  switch(game_editor->tool) {
  case editor_tool::EDIT_DISPLACEMENT:{
    displacement_tool_panel(game_editor, state, window, renderer, mesh_catalog, texture_catalog);   
  }break;
  case editor_tool::EDIT_ENTITY:{
    entity_tool_panel(game_editor, state, window, renderer, mesh_catalog, texture_catalog);
  }break;
  case editor_tool::EDIT_OCEAN:{
    ocean_tool_panel(game_editor, state, window, renderer, mesh_catalog, texture_catalog);    
  }break;
  case editor_tool::MESH_INSTANCE_PANEL:{
    mesh_instance_panel(game_editor, state, renderer, window, mesh_catalog, texture_catalog);    
  }break;
  }

  displacement_editor* displacement_editor = &game_editor->displacement_editor;
  switch(displacement_editor->tool) {
  case displacement_tool::EDIT_TEXTURE:{
    texture_instance_panel(game_editor, state, renderer, window, texture_catalog);   
  }break;
  }
}

internal void
ocean_tool_panel(game_editor* game_editor, game_state* state, game_window* window, ui_handler* ui) {
  game_level* active_level   = game_editor->active_level;
  
  rect initial_rect = {0.0f, 0.0f, 0.2f, 0.45f};
  string panel_title = const_string("Ocean Data");
  region_state* region_state = region_start(ui, UI_ID(0), panel_title, initial_rect, REGION_TITLE);
  
  if(active_level->ocean_data.active) {
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("create water"))) {
      game_editor_add_ocean(game_editor, state, window);
    }
  } else {
    ocean* ocean_data = &active_level->ocean_data;
    
    // #struct_field_for ocean ocean_data
    // property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("struct_field"), &value_name->struct_field); 
  }
  
  region_end(ui);
}

internal void
displacement_tool_panel(game_editor* game_editor, game_state* state, game_window* window, ui_handler* ui) {
  displacement_editor* editor = &game_editor->displacement_editor;
  game_level* active_level = game_editor->active_level;
  
  rect initial_rect = {0, 0, 0.2f, 0.45f};
  string panel_title = const_string("Displacement Editor");
  region_state* region_state = region_start(ui, UI_ID(0), panel_title, initial_rect);
  {
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("New/Delete"))) {
      editor->select_count = 0;
      editor->tool = displacement_tool::EDIT_INSTANCE;
    }
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("Height Edit"))) {
      editor->select_count = 0;
      editor->tool = displacement_tool::EDIT_HEIGHT;
    }
    
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("Texture Edit"))) {
      editor->select_count = 0;
      editor->tool = displacement_tool::EDIT_TEXTURE;
    }
    
    switch(editor->tool) {
    case EDIT_INSTANCE: {
      if(editor->select_count) {
	if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("Instantiate"))) {
	  game_editor_add_displacement(game_editor, window);
      	}
      }
    } break;
    case EDIT_HEIGHT: {
      if(window->mouse_button_left_state & MOUSE_BUTTON_LEFT_UP && window->key_shift_state & KEY_SHIFT_PRESS) {
	// displacement_select_sample_handler(game_editor, window, active_level);
      }
      if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("Update"))) {
	// displacement_vertex_kriging_editor(game_editor, active_level);
      }
      if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("Get Mesh Heights"))) {
	// displacement_generate_sample_array_by_mesh(game_editor, active_level);
      }
      if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("Clear"))) {
	editor->select_count = 0;
	// clear_temporary_pool(editor->sample_pool);
      }
    } break;
    case EDIT_TEXTURE: {
      if(editor->select_count) {
	/*
	u32 displacement_index = editor->select_array[0].value;
	displacement* displacement = get_displacement_by_id(active_level, displacement_index);
	
	// window_state->col_align_count = 2;
	displacement_mesh* mesh_data = &displacement->mesh_data;
	for(u32 i = 0; i < mesh_data->texture_count; i++) {
	  string path = mesh_data->texture_array[i]->path;
	  path.count = 10;
	  if(button(ui, UI_ID(0), get_rect_area(region_state), path)) {
	    
	  }
	}
	if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("+"))) {
	  
	}
	*/
      }
    } break;
    }
    
    region_end(ui);
  }
}

internal void
entity_tool_panel(game_editor* game_editor, game_state* state, game_window* window, ui_handler* ui) {
  game_level* active_level = game_editor->active_level;
  entity* active_entity = NULL;
  
  entity_editor* editor = &game_editor->entity_editor;
  u32 entity_selected_count = editor->entity_selected_count;
  b32 is_selection_active = (entity_selected_count > 0) ? true : false;
  
  string panel_title = {};
  if(is_selection_active) {
    active_entity = get_entity_by_id(active_level, editor->entity_selected_array[0]); // @temporary
    s8 char_id[8] = {};
    stbsp_sprintf(char_id, "%i", active_entity->id);
    
    s8* type_name = entity_type_name[static_cast<u32>(active_entity->type)];
    panel_title   = concatenate(&state->string_arena, 3, type_name, "#", char_id);
  } else {
    panel_title   = concatenate(&state->string_arena, 1, "entity editor"); // @temporary
  }
  
  r32 w = ui->window_layout.x;
  r32 h = ui->window_layout.y;
  rect initial_rect = {w, h, w * 0.2f, h * 0.45f};
  
  region_state* region_state = region_start(ui, UI_ID(0), panel_title, initial_rect, REGION_TITLE);
  {
    if(is_selection_active == false) {
      if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("static_entity"), CLASSIC_BUTTON)) {
	game_editor_add_game_object(game_editor, window, mesh_catalog, texture_catalog, &state->string_arena, renderer, entity_type::STATIC);
      }
      
      if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("square_info"), CLASSIC_BUTTON)) {
	game_editor_add_game_object(game_editor, window, mesh_catalog, texture_catalog, &state->string_arena, renderer, entity_type::SQUARE_INFO);
      }
      
      if(active_level->player_on_level == false && button(ui, UI_ID(0), get_rect_area(region_state), const_string("player_entity"), CLASSIC_BUTTON)) {
	active_level->player_on_level = true;
	game_editor_add_game_object(game_editor, window, mesh_catalog, texture_catalog, &state->string_arena, renderer, entity_type::PLAYER);
      }
    } else {
      entity_type type = active_entity->type;
      
      u32 panel_count    = 0; // @temporary
      u32 panel_flags    = 0;
      u32 property_flags = 0;
      
      if(type == STATIC || type == DYNAMIC) {
	panel_flags = PANEL_BASIC | PANEL_MATERIAL | PANEL_FLAG;
	panel_count = 3;
	
        property_flags |= EDIT_POSITION;
        property_flags |= EDIT_SCALE;
        property_flags |= EDIT_ROTATION;
        property_flags |= EDIT_MESH;
      } else if(type == PLAYER) {
	panel_flags = PANEL_BASIC | PANEL_MATERIAL | PANEL_FLAG;
	panel_count = 3;
	
        property_flags |= EDIT_POSITION;
        property_flags |= EDIT_SCALE;
        property_flags |= EDIT_ROTATION;
      } else if(type == SQUARE_INFO) {
	panel_flags = PANEL_BASIC | PANEL_SQUARE_INFO;
	panel_count = 2;
	
        property_flags |= EDIT_POSITION;
        property_flags |= EDIT_ROTATION;
        property_flags |= PANEL_SQUARE_INFO;
      }
      
      update_div_count(region_state, panel_count);

      basic_style override_style = ui->default_basic_style;
      override_style.active_flags = FILL_LABEL;      

      if((panel_flags & PANEL_BASIC) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("basic"), CLASSIC_BUTTON, &override_style)) {
	ui->editor_active_flags = PANEL_BASIC;
      }
      
      if((panel_flags & PANEL_MATERIAL) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("material"), CLASSIC_BUTTON, &override_style)) {
	ui->editor_active_flags = PANEL_MATERIAL;
      }
      
      if((panel_flags & PANEL_FLAG) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("flag"), CLASSIC_BUTTON, &override_style)) {
	ui->editor_active_flags = PANEL_FLAG;
      }
      
      if((panel_flags & PANEL_SQUARE_INFO) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("link info"), CLASSIC_BUTTON, &override_style)) {
	ui->editor_active_flags = PANEL_SQUARE_INFO;
      }
      
      update_div_count(region_state, 0);
      
      if(ui->editor_active_flags & PANEL_BASIC) {
        if((property_flags & EDIT_POSITION) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("position"), SWITCH_BUTTON)) {
	  property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("x:"), &active_entity->position.x);
	  property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("y:"), &active_entity->position.y);
	  property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("z:"), &active_entity->position.z);
	}
	if((property_flags & EDIT_ROTATION) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("rotation"), SWITCH_BUTTON)) {
	}
	if((property_flags & EDIT_SCALE) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("scale"), SWITCH_BUTTON)) {
	  property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("x:"), &active_entity->scale.x);
	  property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("y:"), &active_entity->scale.y);
	  property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("z:"), &active_entity->scale.z);
	}
	if((property_flags & EDIT_MESH) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("mesh"), SWITCH_BUTTON)) {
	  property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("name:"), &active_entity->target_mesh_name);
	}
	if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("deselect"), CLASSIC_BUTTON)) {
	  write_action(game_editor, action_type::ENTITY_SELECT, recovery_type::DO);
	  memset(editor->entity_selected_array, 0, sizeof(u32) * editor->entity_selected_count);
	  editor->entity_selected_count = 0;	
	}
      }
      
      if(ui->editor_active_flags & PANEL_MATERIAL) {
	u32 material_count = active_entity->material_count;
	
	update_div_count(region_state, material_count);
	for(u32 index = 0; index < material_count; index++) {
	  // @temporary:
	  s8 temp_buffer[32] = {}; 
	  stbsp_sprintf(temp_buffer, "%i", index);
	  string material_id_string = stack_string(temp_buffer);
	  
	  if(button(ui, UI_ID(index), get_rect_area(region_state), material_id_string, CLASSIC_BUTTON, &override_style)) {
	    ui->material_active_index = index;
	  }
	}
	update_div_count(region_state, 0);

	u32 color_id = UI_ID(active_entity->id) * 9 * UI_ID(ui->material_active_index);
	material* material = &active_entity->material_array[ui->material_active_index];
        property_input(ui, UI_ID(color_id), get_rect_area(region_state), const_string("difuse color"), &material->diffuse_color);
      }
      
      if(ui->editor_active_flags & PANEL_SQUARE_INFO) {
	vec3 camera_position = active_level->camera.position;
	square_object* object = static_cast<square_object*>(active_entity->object_pointer);
	
	{
	  u32 active_entity_id = 0;
	  s32 delete_id = -1;
	  
	  for(u32 i = 0; i < object->square_link_count; i++) {
	    u32 current_link = object->square_link_array[i];
	    active_entity_id = current_link;
#if 0 // @incomplete:
	    if(window_property_cursor(ui, window, const_string("link"), &active_entity_id + i)) {
	      vec3 cursor_ray = get_mouse_ray(window->width, window->height, window->mousex, window->mousey, state->persp_matrix, state->view_matrix);
	    
	      if(active_entity_id == 0) {
		delete_id = object->square_link_array[i];
	      } else {
		if(current_link == active_entity_id) { // @note: changed by mouse-pick
		  active_entity_id = get_entity_by_cursor(game_editor, renderer, window, active_level, camera_position);
		}
		
		entity* entity = get_entity_by_id(active_level, active_entity_id);
		if(entity != NULL && entity->type == entity_type::SQUARE_INFO) {
		  square_object* old_link_object = static_cast<square_object*>(get_entity_by_id(active_level, current_link)->object_pointer);
		  square_object* new_link_object = static_cast<square_object*>(get_entity_by_id(active_level, active_entity_id)->object_pointer);
	    
		  delete_link_to_square_object(object, old_link_object);
		  add_link_to_square_object(object,    new_link_object);
		  // edit_link_to_square_object(object, link_object);
		}
	      }
	    }
#endif
	  }
	  
	  if(delete_id >= 0) {
	    square_object* link_object = static_cast<square_object*>(get_entity_by_id(active_level, delete_id)->object_pointer);
	    delete_link_to_square_object(object, link_object);
	  }
	}
	
	{
	  u32 active_entity_id = 0;
#if 0 // @incomplete:
	  if(window_property_cursor(ui, window, const_string("new"), &active_entity_id, &override_theme)) {
	    vec3 cursor_ray = get_mouse_ray(window->width, window->height, window->mousex, window->mousey, state->persp_matrix, state->view_matrix);

	    if(active_entity_id == 0) { 
	      active_entity_id = get_entity_by_cursor(game_editor, renderer, window, active_level, camera_position);
	    }
	    
	    entity* entity = get_entity_by_id(active_level, active_entity_id);
	    if(entity != NULL && entity->type == entity_type::SQUARE_INFO) {
	      square_object* link_object = static_cast<square_object*>(get_entity_by_id(active_level, active_entity_id)->object_pointer);
	      add_link_to_square_object(object, link_object);
	    }
	  }
#endif
	}
      }
    }
  }  
  region_end(ui);
  free_string(&state->string_arena, &panel_title);
}

internal void
recovery_panel(game_editor* game_editor, game_state* state, game_window* window, renderer* renderer, mesh_catalog* mesh_catalog, texture_catalog* texture_catalog) {
  ui_handler* ui = &game_editor->ui_handler;
  
  // @temporary
  r32 w = ui->window_layout.x;
  r32 h = ui->window_layout.y;
  rect initial_rect = {0, 0, w * 0.15f, h * 0.45f};
  
  string panel_title = const_string("recovery");
  region_state* region_state = region_start(ui, UI_ID(0), panel_title, initial_rect, REGION_MOVE | REGION_TITLE);
  region_state->draw_type = SOUTH_TO_NORTH;
  
  {
    basic_style override_style = ui->default_basic_style;
    override_style.active_flags = FILL_LABEL;      
    
    update_draw_rect(region_state, 0.2f);
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("<"), CLASSIC_BUTTON, &override_style)) {
      read_action(game_editor, UNDO);
    }
    
    update_draw_rect(region_state, 0.6f);
    label(ui, get_rect_area(region_state), const_string("last_move"), &override_style);
    
    update_draw_rect(region_state, 0.2f);
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string(">"), CLASSIC_BUTTON, &override_style)) {
      read_action(game_editor, REDO);
    }
  }
  region_end(ui); 
}

internal void
texture_instance_panel(game_editor* game_editor, game_state* game_state, renderer* renderer, game_window* window, texture_catalog* texture_catalog) {
  ui_handler* ui = &game_editor->ui_handler;
  string_arena* string_arena = &game_state->string_arena;
  
  string mesh_title = const_string("texture loader");
  
  r32 w = ui->window_layout.x;
  r32 h = ui->window_layout.y;
  rect r = {0, 0, w * 0.4f, h * 0.35f};
  
  region_state* region_state = region_start(ui, UI_ID(0), mesh_title, r, REGION_MOVE | REGION_TITLE);
  {
    r32 max_width = w * 0.045f;
    u32 col_count = static_cast<u32>(floor(r.w / max_width));
    
    // @temporary:
    string directory_path_array[32] = {};
    string texture_path_array[64]   = {};
    
    u32 directory_count = get_file_count_from_directory(texture_directory_open_path);
    read_all_files_from_directory(string_arena, directory_path_array, texture_directory_open_path);
    
    for(u32 i = 0; i < directory_count; i++) {
      string idirectory_path = string_concatenate(string_arena, 2, texture_directory_path, directory_path_array[i]);
      string open_idirectory_path = string_concatenate(string_arena, 2, idirectory_path, directory_open_token);
      
      u32 texture_file_count = get_file_count_from_directory(open_idirectory_path);
      read_all_files_from_directory(string_arena, texture_path_array, open_idirectory_path);
      
      for(u32 j = 0; j < texture_file_count; j++) {
	// if(state->col_align_count == 0) state->col_align_count = col_count;
	
	string file_name = texture_path_array[j];
	string complete_path = string_concatenate(string_arena, 3, idirectory_path, directory_slash_token, file_name);

	catalog_result result = get_texture_by_path(texture_catalog, string_arena, complete_path);
        texture* current_texture = result.texture;
	
	if(button_texture(ui, UI_ID(0), get_rect_area(region_state), current_texture)) { // @incomplete
	  // break;
	}
	
	// @speedup:
	// remove_texture_from_bucket(texture_catalog, string_arena, current_texture->id);
	free_string(string_arena, &complete_path);
		
	free_string(string_arena, &texture_path_array[j]);
      }
      free_string(string_arena, &idirectory_path);
      free_string(string_arena, &open_idirectory_path);
      
      free_string(string_arena, &directory_path_array[i]);
    }
  }
  region_end(ui); 
}

internal void
mesh_instance_panel(game_editor* game_editor, game_state* game_state, renderer* renderer, game_window* window, mesh_catalog* mesh_catalog, texture_catalog* texture_catalog) {
  ui_handler* ui = &game_editor->ui_handler;
  game_level* active_level   = game_editor->active_level;
  string_arena* string_arena = &game_state->string_arena;

  string mesh_open_file_path = const_string(MESH_OPEN_FILE_PATH);
  string mesh_path_array[64] = {};
  string directory_path_array[32] = {};

  s32 select_directory_index = -1;
  u32 directory_count = get_file_count_from_directory(mesh_open_file_path);
  read_all_files_from_directory(string_arena, directory_path_array, mesh_open_file_path);
  
  string mesh_title = const_string("mesh list");
  r32  w = ui->window_layout.x;
  r32  h = ui->window_layout.y;
  rect r = {w, 0, w * 0.4f, h * 0.3f};
  
  region_state* region_state = region_start(ui, UI_ID(0), mesh_title, r, REGION_MOVE | REGION_TITLE);
  basic_style override_style = ui->default_basic_style;
  override_style.active_flags = FILL_LABEL;      
  update_draw_position(region_state, 0.0f, 0.0f);
  
  update_draw_rect(region_state, 1.0f, 0.1f);
  update_div_count(region_state, 5);
  
  for(u32 idirectory = 0; idirectory < directory_count; idirectory++) {
    push_basic_link(ui, UI_ID(0));
    if(button(ui, UI_ID(idirectory), get_rect_area(region_state), directory_path_array[idirectory], SWITCH_BUTTON, &override_style)) {
      select_directory_index = idirectory;
    }
    pop_basic_link(ui);
  }
  update_div_count(region_state, 0);
  
  update_div_count(region_state, 1);
  update_draw_rect(region_state, 0.5f, 0.1f);
  
  if(select_directory_index != -1) {
    rect scissor_r = region_state->draw_rect;
    scissor_r.w = region_state->draw_rect.w * region_state->drawable_rect.x;
    scissor_r.h = region_state->draw_rect.h * 0.9f;
    
    scroll_state* scroll_state = scroll_start(ui, UI_ID(select_directory_index), scissor_r);
    set_draw_position(region_state, scroll_state->position);
    
    string string_file_path  = const_string(MESH_FILE_PATH);
    string current_directory = string_concatenate(string_arena, 3, string_file_path, directory_path_array[select_directory_index], directory_open_token);
    
    u32 mesh_file_count = get_file_count_from_directory(current_directory);
    read_all_files_from_directory(string_arena, mesh_path_array, current_directory);
    
    for(u32 ifile = 0; ifile < mesh_file_count; ifile++) {
      string mesh_button_title = mesh_path_array[ifile];
      
      if(button(ui, UI_ID(ifile), get_rect_area(region_state), mesh_button_title, CLASSIC_BUTTON, &override_style)) {
	string mesh_file_path  = string_concatenate(string_arena, 4, string_file_path, directory_path_array[select_directory_index], directory_slash_token, mesh_path_array[ifile]);
	
	if(game_editor->mesh_viewer_count) {
	  // clear_mesh_array_loader(game_editor, mesh_catalog, texture_catalog, string_arena, renderer);
	}
	
	load_mesh_array_instance(game_editor, mesh_catalog, texture_catalog, string_arena, renderer, mesh_file_path);
	free_string(string_arena, &mesh_file_path);
      }	  
      free_string(string_arena, &mesh_path_array[ifile]);
    }
    free_string(string_arena, &current_directory);

    scroll_end(ui, scroll_state);
  }
  update_div_count(region_state, 0);
    
  for(u32 idirectory = 0; idirectory < directory_count; idirectory++) {
    free_string(string_arena, &directory_path_array[idirectory]);
  }

  update_draw_position(region_state, 0.5f, 0.1f);
  update_draw_rect(region_state, 0.5f, 0.9f);
      
  r32 view_w = r.w * 0.5f;
  r32 view_h = r.h;
  mat_perspective(game_editor->mesh_proj_matrix, 65.0f, view_w / view_h, 0.1f, 100.0f);
  label_texture(ui, get_rect_area(region_state), renderer->mesh_viewer_texture, {}, &override_style);
  
  /*
    if(point_on_rect(window->mousex, window->mousey, view_r)) {
    const r32 scroll_power = 5.0f;
    game_editor->mesh_camera.radius -= window->scroll * scroll_power * dt;
    game_editor->mesh_camera.radius  = clamp(game_editor->mesh_camera.radius, 5.0f, 0.25f);
    }
  */
  
  update_draw_position(region_state, 0.5f);
  update_draw_rect(region_state, 0.5f, 0.1f);
  // update_spacing_rect(region_state, 0.25f, 0.0f);
  
  if(game_editor->mesh_viewer_count) {
    if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("Instance Mesh"))) { // @temporary @incomplete
      for(u32 i = 0; i < game_editor->mesh_viewer_count; i++) {
	mesh_view* it = &game_editor->mesh_viewer_array[i];

	write_action(game_editor, action_type::ENTITY_ADD_TO_LEVEL, recovery_type::DO);
	entity* new_entity = instantiate_game_object(string_arena, active_level, it->mesh_type);
	  
	apply_mesh_to_entity(mesh_catalog, texture_catalog, string_arena, renderer, new_entity, it->mesh_pointer);
	update_model_matrix(new_entity);
      }
    }
  }
  
  region_end(ui);
}

#if 0
internal void
window_mesh_visualizer(ui_handler* ui, game_editor* game_editor, game_state* game_state, mesh_catalog* mesh_catalog, texture_catalog* texture_catalog, renderer* renderer, game_window* window, rect r, panel_theme* window_theme) {
  game_level* active_level   = game_editor->active_level;
  string_arena* string_arena = &game_state->string_arena; // &mesh_catalog->mesh_path_arena;

  string mesh_title = const_string("mesh pool");
  u32 window_identifier = UID() + pointer_to_int(window_mesh_visualizer);

  window_state* state = window_start(ui, window, mesh_title, r, dt, window_identifier, window_theme, WINDOW_CAN_MOVE | WINDOW_DRAW_TITLE);
  {
    panel_theme override_theme = *window_theme;
    basic_style* button_style  = &override_theme.default_theme.style;
    // button_style->active_color = button_style->color_data.gray_bg_color;
    
    rect list_r = state->rect;
    list_r.w = state->rect.w * 0.5f;

    rect asign_r = state->rect;
    asign_r.w = state->rect.w * 0.5f;
    asign_r.h = asign_r.h * 0.2f;
    asign_r.x = asign_r.x + asign_r.w;
    
    rect view_r = state->rect;
    view_r.w = state->rect.w * 0.5f;
    view_r.h = view_r.h - asign_r.h;
    view_r.x = view_r.x + view_r.w;
    view_r.y = view_r.y + asign_r.h;
    
#if DRAW_DEBUG_GUI
    vec4 rect_color = {1.0f, 1.0f, 0.0f, 1.0f};
    glUniform1f(get_uniform_location(&ui->shader, "u_draw_type"), DRAW_TYPE_COLOR);
    renderer_draw_rect(ui->vao, ui->vbo, ui->ibo, list_r, rect_color);
#endif
    state->rect = list_r;
    
    string mesh_open_file_path      = const_string(MESH_OPEN_FILE_PATH);
    string mesh_path_array[64]      = {};
    string directory_path_array[32] = {};
    
    u32 directory_count = get_file_count_from_directory(mesh_open_file_path);
    read_all_files_from_directory(string_arena, directory_path_array, mesh_open_file_path);
    
    for(u32 i = 0; i < directory_count; i++) {
      button_style->label_flags = RECT_ALL_AXIS | FILL_LABEL;
      if(window_button_title(ui, directory_path_array[i], UID() + i, &override_theme)) {
	string string_file_path  = const_string(MESH_FILE_PATH);
	string open_directory    = const_string("/*");
	string current_directory = string_concatenate(string_arena, 3, string_file_path, directory_path_array[i], open_directory);
	
	u32 mesh_file_count = get_file_count_from_directory(current_directory);
	read_all_files_from_directory(string_arena, mesh_path_array, current_directory);
	
	for(u32 j = 0; j < mesh_file_count; j++) {
	  button_style->label_flags = RECT_WEST_SOUTH | RECT_EAST_SOUTH | CLEAR_WEST_NORTH | CLEAR_EAST_NORTH | FILL_LABEL;
	  
	  if(window_button(ui, mesh_path_array[j], UID() + j, CLASSIC_BUTTON, &override_theme, true)) {
	    string slash_directory = const_string("/");
	    string mesh_file_path  = string_concatenate(string_arena, 4, string_file_path, directory_path_array[i], slash_directory, mesh_path_array[j]);

	    // @temporary: only works for static mesh 
	    if(game_editor->mesh_viewer_pointer) {
	      try_clear_mesh_from_bucket(mesh_catalog, texture_catalog, string_arena, renderer, game_editor->mesh_viewer_pointer, game_editor->mesh_viewer_type); 
	    }

	    entity_type mesh_type;
	    game_editor->mesh_viewer_pointer = get_mesh_from_bucket(mesh_catalog, texture_catalog, string_arena, renderer, mesh_file_path, &mesh_type);
	    game_editor->mesh_viewer_type = mesh_type;
	    free_string(string_arena, &mesh_file_path);
	  }
	  free_string(string_arena, &mesh_path_array[j]);
	}
	free_string(string_arena, &current_directory);
      }
      free_string(string_arena, &directory_path_array[i]);
    }
    
#if DRAW_DEBUG_GUI
    glUniform1f(get_uniform_location(&ui->shader, "u_draw_type"), DRAW_TYPE_COLOR);
    renderer_draw_rect(ui->vao, ui->vbo, ui->ibo, view_r, rect_color);
#endif
    
    { // draw label texture:
      if(point_on_rect(window->mousex, window->mousey, view_r)) {
	r32 scroll_power = 5.0f;
	game_editor->mesh_camera.radius -= window->scroll * scroll_power * dt;
	game_editor->mesh_camera.radius  = clamp(game_editor->mesh_camera.radius, 5.0f, 0.25f);
      }
      
      mat_perspective(game_editor->mesh_proj_matrix, 65.0f, (r32) view_r.w / (r32) view_r.h, 0.1f, 100.0f);
      
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, renderer->mesh_viewer_texture);
      glUniform1f(get_uniform_location(&ui->shader, "u_draw_type"), DRAW_TYPE_TEXTURE);
      
      vec4 rect_color = {1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_rect(ui->vao, ui->vbo, ui->ibo, view_r, rect_color);
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    if(game_editor->mesh_viewer_pointer) {
      if(button(ui, window, asign_r, dt, const_string("Asign Model"), CLASSIC_BUTTON, __LINE__)) {
	entity_editor* editor = &game_editor->entity_editor;
	for(u32 i = 0; i < editor->entity_selected_count; i++) {
	  entity* it = get_entity_by_id(active_level, editor->entity_selected_array[i]);
	  write_action(game_editor, action_type::ENTITY_MESH_CHANGE, recovery_type::DO);

	  // @temporary: change entity type and game_object type
	  if(it->type != PLAYER && game_editor->mesh_viewer_type != it->type) {
	    // @incomplete:
	    entity temp_entity_values;
	    copy_entity_values(&temp_entity_values, it);
	    remove_entity_from_level(active_level, mesh_catalog, texture_catalog, string_arena, renderer, it);

	    entity* new_entity = instantiate_game_object(active_level, game_editor->mesh_viewer_type);
	    copy_entity_values(new_entity, &temp_entity_values);
	    
	    new_entity->type = game_editor->mesh_viewer_type;
	    
	    it = new_entity;
	  }
	  
	  string mesh_path = get_mesh_path(game_editor->mesh_viewer_pointer, game_editor->mesh_viewer_type);
	  void* mesh_to_asign = get_mesh_from_bucket(mesh_catalog, texture_catalog, string_arena, renderer, mesh_path);
	  apply_mesh_to_entity(mesh_catalog, texture_catalog, string_arena, renderer, it, mesh_to_asign);
	}

	// @temporary: clear all entity selected count to prevent select deleted objets
	b32 do_clear = false;
	for(u32 i = 0; i < editor->entity_selected_count; i++) {
	  entity* it = get_entity_by_id(active_level, editor->entity_selected_array[i]);
	  if(it == NULL) {
	    do_clear = true;
	    break;
	  }
	}
	if(do_clear) {
	  editor->entity_selected_count = 0;
	}
      }
    }
  }
  window_end(ui);
}
#endif

#if 0
internal void
displacement_tool_panel(game_editor* game_editor, game_state* state, game_window* window, renderer* renderer, mesh_catalog* mesh_catalog, texture_catalog* texture_catalog) {
  game_level* active_level = game_editor->active_level;
  ui_handler* ui = &game_editor->ui_handler;
  
  panel_theme theme = ui->default_panel_theme;
  // theme.button_theme.border_flags     = RECT_ALL_AXIS;
  // theme.text_input_theme.border_flags = RECT_ALL_AXIS;
  
  panel_layout layout = default_panel(window, 0.25f);
  r32 y_offset = 0; // @temporary
  y_offset += layout.item_size.y;
  
  displacement_editor* editor = &game_editor->displacement_editor;
  
  rect r;
  r.w = layout.item_size.x;
  r.h = layout.item_size.y;
  r.x = window->width  - r.w;
  r.y = window->height - r.h;  
  
  rect header_r = r;
  header_r.w = r.w * 0.33f;
  header_r.x = r.x + header_r.w * 0;
  if(button(ui, window, header_r, dt, const_string("Select"), __LINE__, &theme)) {
    game_editor->displacement_editor.tool = displacement_tool::EDIT_SELECTION;
  }
  header_r.x = r.x + header_r.w * 1;
  if(button(ui, window, header_r, dt, const_string("Vertex"), __LINE__, &theme)) {
    game_editor->displacement_editor.tool = displacement_tool::EDIT_VERTEX;
  }
  header_r.x = r.x + header_r.w * 2;
  if(button(ui, window, header_r, dt, const_string("Blend"), __LINE__, &theme)) {
    game_editor->displacement_editor.tool = displacement_tool::EDIT_BLEND;
  }
  
  switch(editor->tool) {
  case displacement_tool::EDIT_SELECTION: {
  }break;
  case displacement_tool::EDIT_VERTEX: {
    r.y -= layout.item_size.y;
    if(button(ui, window, r, dt, const_string("New Kriging"), __LINE__, &theme)) {
      if(editor->kriging_data_count < HEIGHT_DATA_MAX) {
	editor->kriging_data_count++;
	editor->active_kriging_index = 0;
      }
    }

    r.y -= layout.item_size.y;
    if(button(ui, window, r, dt, const_string("update"), __LINE__, &theme)) {
      displacement_vertex_kriging_editor(game_editor, active_level);
    }

    r.y -= layout.item_size.y;
    if(button(ui, window, r, dt, const_string("Add Sample"), __LINE__, &theme)) {
      displacement_sample_add(editor);
    }
  }break;
  case displacement_tool::EDIT_BLEND: {
    r.y -= layout.item_size.y;
    if(button(ui, window, r, dt, const_string("New Blending"), __LINE__, &theme)) {
      if(editor->blend_data_count < BLEND_DATA_MAX) {
	editor->blend_data_count++;
	editor->active_blend_index = 0;
      }
    }
    
    // @incomplete!
    if(editor->blend_data_count > 0) {
      r.y -= layout.item_size.y;
      if(button(ui, window, r, dt, const_string("Update"), __LINE__, &theme)) {
	displacement_vertex_blend_editor(game_editor, active_level);
      }
      
      r.y -= layout.item_size.y;
      for(u32 i = 0; i < editor->blend_data_count; i++) {
	rect item_r = r;
	item_r.w = item_r.w * (1.0f / static_cast<r32>(BLEND_DATA_MAX));
	item_r.x = item_r.x + (item_r.w * static_cast<r32>(i));

	s8 char_item = '0' + i;
	string title = const_string(&char_item);
	if(button(ui, window, item_r, dt, title, __LINE__ * i, &theme)) { // @temporary
	  editor->active_blend_index = i;
	}
      }
      
      blend_data* data = &editor->blend_data_array[editor->active_blend_index];
      r.y -= layout.item_size.y;
      if(button(ui, window, r, dt, const_string("Add Sample"), __LINE__, &theme)) {
	u32 index = data->sample_point_count; 
	if(data->sample_point_count < BLEND_SAMPLES_MAX) {
	  data->sample_point_count++;
	}
      }
      
      real_value_editor_ui(ui, window, dt, __LINE__, r, const_string("Range"), &data->range, &theme, 0);
    } 
  }break;
  }  
}
#endif
#endif // !EDITOR_PANEL_H
