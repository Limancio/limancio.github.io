
internal void
entity_tool_panel(game_editor* game_editor, game_window* window, game_state* state, ui_handler* ui) {
  entity_editor* editor    = &game_editor->entity_editor;
  game_level* active_level = game_editor->active_level;

  u32 entity_count = editor->entity_selected_count;

  string panel_title;
  if(entity_count) {
    if(entity_count == 1) {
      u32 id = editor->entity_selected_array[0];
      entity* e = get_entity_by_id(active_level, id);
      
      s8 entity_id[8] = {};
      s8* entity_type = entity_type_names[static_cast<u32>(e->type)];
      stbsp_sprintf(entity_id, "%i", e->id);
      
      concatenate(ui->string_list, panel_title, 3, entity_type, "#", entity_id);      
    } else {
      s8 count[8] = {};
      stbsp_sprintf(count, "%i", entity_count);
      
      concatenate(ui->string_list, panel_title, 2, "entity_array#", count);
    }
  } else {
    string_alloc(ui->string_list, panel_title, "entities");
  }
  
  r32 w = window->width;
  r32 h = window->height;
  rect initial_rect = {w, h, w * 0.2f, h * 0.45f};
  
  region_style panel_style = {ui->default_basic_style, ui->default_basic_style};

  // @incomplete: move to region_header_style function?
  panel_style.header.active_light_color = {0.45f, 0.15f, 0.2f, 1.0f};
  panel_style.header.active_dark_color  = {0.015f, 0.1f, 0.2f, 1.0f};
  panel_style.header.active_flags = ALL_CLEAR;
  
  region_state* region_state = region_start(ui, UI_ID(0), panel_title, initial_rect, REGION_TITLE, &panel_style);
  {
    if(entity_count == 0) {
      if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("static_entity"), CLASSIC_BUTTON)) {
	game_editor_add_game_object(game_editor, window, STATIC_OBJECT);
      }
      
      if(active_level->player_on_level == false && button(ui, UI_ID(0), get_rect_area(region_state), const_string("player_entity"), CLASSIC_BUTTON)) {
	active_level->player_on_level = true;
	game_editor_add_game_object(game_editor, window, PLAYER_OBJECT);
      }
    } else {
      u32 panel_count    = 0; // @temporary
      u32 panel_flags    = 0;
      u32 property_flags = 0;
      
      for(u32 i = 0; i < entity_count; i++) {
	u32 id = editor->entity_selected_array[i];
	entity* it = get_entity_by_id(active_level, id);
	
	entity_type type = it->type;
	
	if(type == STATIC_OBJECT || type == DYNAMIC_OBJECT) {
	  panel_flags |= PANEL_BASIC | PANEL_MATERIAL | PANEL_FLAG;
	  panel_count = 3;
	
	  property_flags |= EDIT_POSITION;
	  property_flags |= EDIT_SCALE;
	  property_flags |= EDIT_ROTATION;
	  property_flags |= EDIT_MESH;
	} else if(type == PLAYER_OBJECT) {
	  panel_flags |= PANEL_BASIC | PANEL_MATERIAL | PANEL_FLAG;
	  panel_count = 3;
	
	  property_flags |= EDIT_POSITION;
	  property_flags |= EDIT_SCALE;
	  property_flags |= EDIT_ROTATION;
	} else if(type == SQUARE_INFO_OBJECT) {
	  panel_flags |= PANEL_BASIC | PANEL_SQUARE_INFO;
	  if(panel_count < 2) {
	    panel_count = 2;
	  }
	
	  property_flags |= EDIT_POSITION;
	  property_flags |= EDIT_ROTATION;
	  property_flags |= PANEL_SQUARE_INFO;
	}
      }
      
      {
	update_div_count(region_state, panel_count);
	basic_style override_style = ui->default_basic_style;
	override_style.active_flags = FILL_LABEL;      
	
	if((panel_flags & PANEL_BASIC) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("basic"), CLASSIC_BUTTON, &override_style)) {
	  game_editor->panel_active_flags = PANEL_BASIC;
	}
	
	if((panel_flags & PANEL_MATERIAL) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("material"), CLASSIC_BUTTON, &override_style)) {
	  game_editor->panel_active_flags = PANEL_MATERIAL;
	}
	
	if((panel_flags & PANEL_FLAG) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("flag"), CLASSIC_BUTTON, &override_style)) {
	  game_editor->panel_active_flags = PANEL_FLAG;
	}
	
	if((panel_flags & PANEL_SQUARE_INFO) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("link info"), CLASSIC_BUTTON, &override_style)) {
	  game_editor->panel_active_flags = PANEL_SQUARE_INFO;
	}
	
	update_div_count(region_state, 0);
      }
      
      u32 position_id = UI_ID(0);
      u32 rotation_id = UI_ID(0);
      u32 scale_id    = UI_ID(0);
      if(entity_count == 1) {
	if(game_editor->panel_active_flags & PANEL_BASIC) {
	  u32 id = editor->entity_selected_array[0];
	  entity* it = get_entity_by_id(active_level, id);
	  
	  if((property_flags & EDIT_POSITION) && button(ui, position_id, get_rect_area(region_state), const_string("position"), SWITCH_BUTTON)) {
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("x:"), &it->position.x);
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("y:"), &it->position.y);
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("z:"), &it->position.z);
	  }
	  
	  if((property_flags & EDIT_ROTATION) && button(ui, rotation_id, get_rect_area(region_state), const_string("rotation"), SWITCH_BUTTON)) {
	    vec3 euler = euler_from_quat(it->rotation);
	    euler.x = to_degrees(euler.x);
	    euler.y = to_degrees(euler.y);
	    euler.z = to_degrees(euler.z);
	    
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("x:"), &euler.x);
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("y:"), &euler.y);
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("z:"), &euler.z);
	  }
	  
	  if((property_flags & EDIT_SCALE) && button(ui, scale_id, get_rect_area(region_state), const_string("scale"), SWITCH_BUTTON)) {
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("x:"), &it->scale.x);
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("y:"), &it->scale.y);
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("z:"), &it->scale.z);
	  }
	        
	  string temp_string = static_to_string(it->target_mesh_name);
	  // string_append(temp_string, mesh_data.name);
      
	  if((property_flags & EDIT_MESH) && button(ui, UI_ID(0), get_rect_area(region_state), const_string("mesh"), SWITCH_BUTTON)) {
	    if(property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("name:"), &temp_string)) {
	      mesh_file_data file_data = {};
	      read_mesh_file(&file_data, game_editor->catalog_handler, temp_string);
	      if(file_data.array_count) {
		
		mesh_data mesh_data = file_data.mesh_data_array[0];
		renderer_submit_mesh(&mesh_data, &it->mesh_render);
		
	      }
	      // clear_mesh_file(&file_data); @incomplete
	      
	    }
	  }
	  update_static_string(it->target_mesh_name, temp_string);
      
	}
      
      } else {
	vec3 center_position = get_center_position(editor, active_level);
	
	if(game_editor->panel_active_flags & PANEL_BASIC) {
	  if((property_flags & EDIT_POSITION) && button(ui, position_id, get_rect_area(region_state), const_string("center position"), SWITCH_BUTTON)) {
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("x:"), &center_position.x);
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("y:"), &center_position.y);
	    property_input(ui, UI_ID(0), get_rect_area(region_state), const_string("z:"), &center_position.z);
	  }
	}
      }

      if(button(ui, UI_ID(0), get_rect_area(region_state), const_string("deselect"), CLASSIC_BUTTON)) {
	write_action(game_editor, action_type::ENTITY_SELECT, recovery_type::DO);
	memset(editor->entity_selected_array, 0, sizeof(u32) * editor->entity_selected_count);
	
	editor->entity_selected_count = 0;	
      }
      
#if 0
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
#endif
      
    }
  }
  region_end(ui);
  
  string_free(ui->string_list, panel_title);
}

internal void
list_tool_panel(game_editor* game_editor, game_window* window, game_state* state, ui_handler* ui) {
  r32 w = window->width;
  r32 h = window->height;
  rect initial_rect = {0, h, w * 0.15f, h * 0.45f};
  
  string panel_title = game_editor->active_level->level_name;
  region_style panel_style = {ui->default_basic_style, ui->default_basic_style};

  // @incomplete: move to region_header_style function?
  panel_style.header.active_light_color = {0.45f, 0.15f, 0.2f, 1.0f};
  panel_style.header.active_dark_color  = {0.015f, 0.1f, 0.2f, 1.0f};
  panel_style.header.active_flags = ALL_CLEAR;
  
  region_state* region_state = region_start(ui, UI_ID(0), panel_title, initial_rect, REGION_TITLE, &panel_style);
  {
    basic_style override_style = ui->default_basic_style;
    
    spacing(region_state, 0.5f);
    
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
  
  // recovery_panel(game_editor, state, window, renderer, mesh_catalog, texture_catalog);  
  switch(game_editor->tool) {
  case editor_tool::EDIT_DISPLACEMENT:{
  } break;
  case editor_tool::EDIT_ENTITY:{
    entity_tool_panel(game_editor, window, state, ui);
  } break;
  case editor_tool::EDIT_OCEAN:{
  } break;
  case editor_tool::MESH_INSTANCE_PANEL:{
  } break;
  }
  
  displacement_editor* displacement_editor = &game_editor->displacement_editor;
  switch(displacement_editor->tool) {
  case displacement_tool::EDIT_TEXTURE:{
  }break;
  }
}
