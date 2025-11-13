#ifndef RECOVERY_HANDLER_H
#define RECOVERY_HANDLER_H

// recovery action type
//
// REMBEMBER TO ADD THE NEW ACTION TYPE BELOW!
//
enum action_type {
  SELECT_DISPLACEMENT,
  MOVE_DISPLACEMENT,
  MOVE_VERTEX,

  // ENTITIES
  ENTITY_SELECT,
  ENTITY_TRANSLATE,
  ENTITY_ROTATE,
  ENTITY_SCALE,

  ENTITY_MESH_CHANGE,
  ENTITY_INSTANTIATE,
  ENTITY_REMOVE
};

char *action_type_names[] = {
  "SELECT_DISPLACEMENT",
  "MOVE_DISPLACEMENT",
  "MOVE_VERTEX",
  
  "ENTITY_SELECT",
  "ENTITY_TRANSLATE",
  "ENTITY_ROTATE",
  "ENTITY_SCALE",
  
  "ENTITY_MESH_CHANGE",
  "ENTITY_ADD_TO_LEVEL",
  "ENTITY_REMOVE_FROM_LEVEL"
};

#define increment_buffer_index(current_index, value_size) current_index += sizeof(value_size) / sizeof(u8)

internal void
recovery_handler_log(recovery_handler* handler) {
  s8* buffer_data = handler->buffer.buffer;
  
  u32 last_save_size;
  size_t last_save_index = handler->buffer_cursor - sizeof(u32); 
  copy_value_at(&handler->buffer, &last_save_size, sizeof(u32), last_save_index);
  
  u32 data_size;
  action_type action_type;
  
  size_t index = handler->buffer_cursor - last_save_size;
  copy_value_at(&handler->buffer, &data_size, sizeof(u32), index);
  copy_value_at(&handler->buffer, &action_type, sizeof(action_type), index);
  
  log_info("recovery LOG >> %zi/%zi %s\n", handler->changes_cursor, handler->changes_count, action_type_names[(u32) action_type]);
}

internal void
write_action(game_editor* game_editor, action_type action, recovery_type type) {
  recovery_handler* handler = &game_editor->recovery_handler;
  
  //
  // recovery_buffer layout
  // [data_size, action_type, %variable_data%, data_size(again)]
  //
  
  dormammu_buffer* write_buffer = &handler->temp_buffer;
  size_t write_data_size = 0;
  
  feed_dormammu(write_buffer, &write_data_size, sizeof(u32));
  feed_dormammu(write_buffer, &action, sizeof(action_type));
  
  switch(action) {
  case ENTITY_SELECT: {
    entity_editor* editor = &game_editor->entity_editor;
    
    feed_dormammu(write_buffer, &editor->entity_selected_count, sizeof(u32));
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      feed_dormammu(write_buffer, &editor->entity_selected_array[i], sizeof(u32));
    }
  }break;
  case ENTITY_TRANSLATE: {
    entity_editor* editor = &game_editor->entity_editor;
    
    feed_dormammu(write_buffer, &editor->entity_selected_count, sizeof(u32));
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity = get_entity_by_id(game_editor->active_level, id);
      
      feed_dormammu(write_buffer, &entity->position, sizeof(vec3));
    }
  }break;
  case ENTITY_ROTATE: {
    entity_editor* editor = &game_editor->entity_editor;
    
    feed_dormammu(write_buffer, &editor->entity_selected_count, sizeof(u32));
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity = get_entity_by_id(game_editor->active_level, id);
      
      feed_dormammu(write_buffer, &entity->rotation, sizeof(quat));
    }
  }break;
  case ENTITY_SCALE: {
    entity_editor* editor = &game_editor->entity_editor;
    
    feed_dormammu(write_buffer, &editor->entity_selected_count, sizeof(u32));
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity = get_entity_by_id(game_editor->active_level, id);
      
      feed_dormammu(write_buffer, &entity->scale, sizeof(vec3));
    }
  }break;
  case ENTITY_MESH_CHANGE: {
    entity_editor* editor = &game_editor->entity_editor;
    
    feed_dormammu(write_buffer, &editor->entity_selected_count, sizeof(u32));
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity = get_entity_by_id(game_editor->active_level, id);
      
      // @incomplete:
    }
  }break;
  case ENTITY_INSTANTIATE: {
    game_level* active_level = game_editor->active_level;
    
    feed_dormammu(write_buffer, &active_level->next_entity_id, sizeof(u32));
  }break;
  case ENTITY_REMOVE: {
    entity_editor* editor = &game_editor->entity_editor;
    game_level* active_level = game_editor->active_level;
    
    u32 remove_count = editor->entity_selected_count;
    feed_dormammu(write_buffer, &remove_count, sizeof(u32));
    for(u32 i = 0; i < remove_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity_pointer = get_entity_by_id(game_editor->active_level, id);
      
      // @incomplete: parse entity data to buffer
    }
  }break;
  }

  size_t write_data_at = write_buffer->cursor_index;
  feed_dormammu(write_buffer, &write_data_size, sizeof(u32));
  
  write_data_size = write_buffer->cursor_index;
  memcpy(&write_buffer->buffer[0], &write_data_size, sizeof(size_t));
  memcpy(&write_buffer->buffer[write_data_at], &write_data_size, sizeof(size_t));
  
  size_t write_index = handler->buffer.cursor_index;
  if(type == recovery_type::DO) {
    if(handler->buffer_cursor < get_buffer_size(&handler->buffer)) {
      handler->changes_count = handler->changes_cursor;
    }
    
    set_buffer_index(&handler->buffer, handler->buffer_cursor);
    feed_dormammu(&handler->buffer, write_buffer);
    handler->buffer_cursor += write_data_size;
    
    handler->changes_count += 1;
    handler->changes_cursor = handler->changes_count;
  } else {
    size_t undo_data_size;
    size_t undo_data_index = handler->buffer_cursor;
    copy_value_at(&handler->buffer, &undo_data_size, sizeof(u32), undo_data_index);
    
    size_t remaining_data_index = handler->buffer_cursor + undo_data_size;
    size_t remaining_data_size  = get_buffer_size(&handler->buffer) - remaining_data_index;
    feed_dormammu(write_buffer, &handler->buffer.buffer[remaining_data_index], remaining_data_size);
    
    set_buffer_index(&handler->buffer, handler->buffer_cursor);
    feed_dormammu(&handler->buffer, write_buffer);
  }
  
  handler->can_undo = handler->buffer_cursor > 0;
  handler->can_redo = handler->changes_cursor < handler->changes_count;
  clear_dormammu(&handler->temp_buffer);
  
  recovery_handler_log(handler);
}

internal void
read_action(game_editor* game_editor, recovery_type type) {
  recovery_handler* handler = &game_editor->recovery_handler;
  
  //
  // recovery_buffer layout
  // [data_size, is_undo_save, action_type, %variable_data%, data_size (again)]
  //
  
  dormammu_buffer* write_buffer = &handler->buffer;
  size_t undo_index;
  size_t undo_data_size;
  action_type undo_action;
    
  if(type == recovery_type::UNDO) {
    if(!handler->can_undo) {
      return; 
    }
    
    //
    // [undo] [undo] [undo]/[redo]/[]...
    //              ^
    //	       index_cursor
    //
    // [undo] [undo] []
    //       ^------x
    //
    
    u32 undo_buffer_size;
    write_buffer = &handler->temp_buffer;
    
    size_t start_index = handler->buffer_cursor - sizeof(u32);
    copy_value_at(&handler->buffer, &undo_buffer_size, sizeof(u32), start_index);
    
    //
    //       write redo data
    //              v
    // [undo] [undo] []...
    //       ^------x
    //  	index_cursor
    //
    undo_index = handler->buffer_cursor - undo_buffer_size;
    copy_value_at(&handler->buffer, write_buffer, undo_buffer_size, undo_index);
    copy_value_at(write_buffer, &undo_action, sizeof(action_type), undo_index);
    
    handler->changes_cursor -= 1;
    handler->buffer_cursor  -= undo_buffer_size;
  } else {
    if(!handler->can_redo) {
      return; 
    }
    
    //
    // [undo] [redo to read]
    //       ^
    //	index_cursor
    //
    
    undo_index = handler->buffer_cursor;
    copy_value_at(write_buffer, &undo_data_size, sizeof(u32), undo_index); // @incomplete
    copy_value_at(write_buffer, &undo_action, sizeof(action_type), undo_index);
  
    handler->changes_cursor += 1;
    handler->buffer_cursor  += undo_data_size;
  }
  
  switch(undo_action) {
  case ENTITY_SELECT: {
    entity_editor* editor = &game_editor->entity_editor;
    
    copy_value_at(write_buffer, &editor->entity_selected_count, sizeof(u32), undo_index);
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 entity_id;
      copy_value_at(write_buffer, &entity_id, sizeof(u32), undo_index);
      
      editor->entity_selected_array[i] = entity_id;
    }
  }break;
  case ENTITY_TRANSLATE: {
    entity_editor* editor = &game_editor->entity_editor;
    
    copy_value_at(write_buffer, &editor->entity_selected_count, sizeof(u32), undo_index);
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity = get_entity_by_id(game_editor->active_level, id);

      vec3 entity_position;
      copy_value_at(write_buffer, &entity_position, sizeof(vec3), undo_index);
      
      entity->position = entity_position;
      update_model_matrix(entity);
    }
  }break;
  case ENTITY_ROTATE: {
    entity_editor* editor = &game_editor->entity_editor;
    
    copy_value_at(write_buffer, &editor->entity_selected_count, sizeof(u32), undo_index);
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity = get_entity_by_id(game_editor->active_level, id);
      
      quat entity_rotation;
      copy_value_at(write_buffer, &entity_rotation, sizeof(vec3), undo_index);
      
      entity->rotation = entity_rotation;
      update_model_matrix(entity);
    }
  }break;
  case ENTITY_SCALE: {
    entity_editor* editor = &game_editor->entity_editor;
    
    copy_value_at(write_buffer, &editor->entity_selected_count, sizeof(u32), undo_index);
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity = get_entity_by_id(game_editor->active_level, id);
      
      vec3 entity_scale;
      copy_value_at(write_buffer, &entity_scale, sizeof(vec3), undo_index);
      
      entity->scale = entity_scale;
      update_model_matrix(entity);
    }
  }break;
  case ENTITY_MESH_CHANGE: {
    entity_editor* editor = &game_editor->entity_editor;
    
    copy_value_at(write_buffer, &editor->entity_selected_count, sizeof(u32), undo_index);
    for(u32 i = 0; i < editor->entity_selected_count; i++) {
      u32 id = editor->entity_selected_array[i];
      entity* entity = get_entity_by_id(game_editor->active_level, id);
      
      // @incomplete:
      // clear_mesh_from_entity(handler->mesh_catalog_pointer, handler->texture_catalog_pointer, handler->string_arena_pointer, handler->renderer_pointer, entity);
      // void* old_mesh = load_mesh_by(handler->string_arena_pointer, handler->renderer_pointer, handler->mesh_catalog_pointer, handler->texture_catalog_pointer, mesh_path);
      // apply_mesh_to_entity(handler->mesh_catalog_pointer, handler->texture_catalog_pointer, handler->string_arena_pointer, handler->renderer_pointer, entity, old_mesh);
    }
  }break;
  case ENTITY_INSTANTIATE: {
    game_level* active_level = game_editor->active_level;    
    
    u32 entity_id;
    copy_value_at(write_buffer, &entity_id, sizeof(u32), undo_index);
    
    entity* entity_pointer = get_entity_by_id(active_level, entity_id);
    remove_entity(active_level, entity_pointer);
  }break;
  case ENTITY_REMOVE: {
    entity_editor* editor = &game_editor->entity_editor;
    game_level* active_level = game_editor->active_level;
    
    u32 remove_count;
    copy_value_at(write_buffer, &remove_count, sizeof(u32), undo_index);
    for(u32 i = 0; i < remove_count; i++) {
      // @temporary:
      instantiate_entity(active_level, entity_type::STATIC_OBJECT);
    }
  }break;
  }
  
  if(type == recovery_type::UNDO) {
    clear_dormammu(&handler->temp_buffer);
    write_action(game_editor, undo_action, recovery_type::REDO);    
  }
  
  handler->can_undo = handler->buffer_cursor > 0;
  handler->can_redo = handler->changes_cursor < handler->changes_count;
  clear_dormammu(&handler->temp_buffer);
  
  recovery_handler_log(handler);
}

#endif // !RECOVERY_HANDLER_H
