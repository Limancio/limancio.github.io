#include "game.h"
#include "game_editor.h"

internal void
game_state_update(game_state* state, game_window* window, r32 dt) {
  game_level* level = &state->level;
  game_camera* camera = &level->camera;
  
  level->ocean_data.water_move_elapsed += level->ocean_data.wave_speed * dt;
  
  if(level->player_on_level) {
    player* player = &level->player;
    entity* player_entity = get_entity_by_id(level, player->entity_id);

    b32 do_move = false; 
    if(window->press_keys[GLFW_KEY_W] || window->press_keys[GLFW_KEY_S] || window->press_keys[GLFW_KEY_A] || window->press_keys[GLFW_KEY_D]) {
      do_move = true;
    }
    
    if(window->press_keys[GLFW_KEY_W]) { player->target_delta = 0.0f;  }
    if(window->press_keys[GLFW_KEY_S]) { player->target_delta = 180.0f; }
    if(window->press_keys[GLFW_KEY_A]) { player->target_delta = 90.0f; }
    if(window->press_keys[GLFW_KEY_D]) { player->target_delta = -90.0f;   }
    
    if(window->press_keys[GLFW_KEY_W] && window->press_keys[GLFW_KEY_A]) { player->target_delta =  45.0f;  }
    if(window->press_keys[GLFW_KEY_W] && window->press_keys[GLFW_KEY_D]) { player->target_delta = -45.0f;  }
    if(window->press_keys[GLFW_KEY_S] && window->press_keys[GLFW_KEY_A]) { player->target_delta =  135.0f; }
    if(window->press_keys[GLFW_KEY_S] && window->press_keys[GLFW_KEY_D]) { player->target_delta = -135.0f; }
    
    r32 delta = player->target_delta;
    
    vec3 rotation_euler = {0, to_radians(delta), 0};
    quat rotation_quat = quat_from_euler(rotation_euler);
    player_entity->rotation = rotation_quat;
    
    if(do_move) {
      r32 dx = (r32) (player->move_speed * dt * sin(to_radians(delta)));
      r32 dz = (r32) (player->move_speed * dt * cos(to_radians(delta)));
      vec3 delta_position = {dx, 0, dz};
      
      player_entity->position = player_entity->position + delta_position;      
    }

    update_model_matrix(player_entity);
  }
}

internal void
remove_entity(game_level* level, entity* entity) {
  u8* array_pointer;
  u32* array_count;
  size_t struct_size;
  
  switch(entity->type) {
  case entity_type::STATIC_OBJECT: {
    array_pointer = reinterpret_cast<u8*>(level->static_object_array);
    array_count   = &level->static_object_count;
    struct_size   = sizeof(static_object);
  } break;
  case entity_type::DYNAMIC_OBJECT: {
    array_pointer = reinterpret_cast<u8*>(level->dynamic_object_array);
    array_count   = &level->dynamic_object_count;
    struct_size   = sizeof(dynamic_object);    
  } break;
  case entity_type::SQUARE_INFO_OBJECT: {
    array_pointer = reinterpret_cast<u8*>(level->square_object_array);
    array_count   = &level->square_object_count;
    struct_size   = sizeof(square_object);    
  } break;
  }
  
  u32 index_to_delete;
  for(size_t i = 0; i < *array_count; i++) {
    size_t buffer_index = (struct_size * i); 
    
    u32* id_pointer = reinterpret_cast<u32*>(&array_pointer[buffer_index]);
    
    if(entity->id == *id_pointer) {
      index_to_delete = *id_pointer;
      break;
    }
  }
  
  u32 new_object_count = *array_count - 1;
  if(new_object_count != index_to_delete && new_object_count > 0) {
    size_t buffer_index = (struct_size * new_object_count); 
    memcpy(&array_pointer[index_to_delete], &array_pointer[index_to_delete + 1], GAME_OBJECT_COUNT - (index_to_delete + 1));
  }
  
  // clear_mesh_from_entity(mesh_catalog, texture_catalog, string_arena, renderer, entity); // @temporary: checks entity type again :(
  *array_count = new_object_count;
  hmdel(level->entity_array, entity->id);
}

internal entity*
add_entity_to_level(game_level* level) { // @incomplete
  entity* new_entity = NULL;
  
  // @temporary: basic id generation
  u32 id = level->next_entity_id;
  level->next_entity_id += 1;
  
  printf("new entity id: %i\n", id);
  hmput(level->entity_array, id, entity{});
  new_entity = &hmget(level->entity_array, id);
  new_entity->id = id;
  
  return(new_entity); 
}

internal displacement*
instantiate_displacement(game_level* level, u32 index) {
  displacement* new_displacement = 0;
  
  hmput(level->displacement_array, index, displacement{});
  new_displacement = &hmget(level->displacement_array, index);
  return(new_displacement);
}

internal entity*
instantiate_entity(game_level* level, entity_type type) {
  entity* new_entity = add_entity_to_level(level);
  new_entity->type = type;
  // new_entity->target_mesh_name = alloc_string(string_arena, MAX_PATH);
  
  u8* array_pointer;
  u32* array_count;
  size_t struct_size;
  
  switch(type) {
  case entity_type::STATIC_OBJECT: {
    array_pointer = reinterpret_cast<u8*>(level->static_object_array);
    array_count   = &level->static_object_count;
    struct_size   = sizeof(static_object);
  } break;
  case entity_type::DYNAMIC_OBJECT: {
    array_pointer = reinterpret_cast<u8*>(level->dynamic_object_array);
    array_count   = &level->dynamic_object_count;
    struct_size   = sizeof(dynamic_object);    
  } break;
  case entity_type::SQUARE_INFO_OBJECT: {
    array_pointer = reinterpret_cast<u8*>(level->square_object_array);
    array_count   = &level->square_object_count;
    struct_size   = sizeof(square_object);    
  } break;
  }
  
  u32 object_index = *array_count;
  *array_count += 1;
  
  size_t buffer_index = (struct_size * object_index); 
  u8* object_pointer = &array_pointer[buffer_index];
  
  u32* entity_id = reinterpret_cast<u32*>(object_pointer);
  
  *entity_id = new_entity->id;
  new_entity->object_pointer = object_pointer;
  
  return(new_entity);
}

internal void
get_joint_transform(r32* out_transform, joint_animation* it, joint* joint, r32 dt) {
  key_frame current_frame = it->frame_array[it->current_frame_index];
  key_frame next_frame = it->frame_array[it->current_frame_index+1];
  
  it->elapsed_time += dt * 1.0f;
  if((current_frame.time_stamp + it->elapsed_time) >= next_frame.time_stamp) {
    it->elapsed_time = 0.0f;
    if(it->current_frame_index+1 < it->frame_count) {
      it->current_frame_index++;
    } else {
      it->current_frame_index = 0;
    }
    current_frame = it->frame_array[it->current_frame_index];
    next_frame    = it->frame_array[it->current_frame_index+1];
  }
  
  r32 progression = it->elapsed_time / (next_frame.time_stamp - current_frame.time_stamp);
  // printf("%i to %i (%f)\n", it->current_frame_index, it->current_frame_index+1, progression);

  vec3 interpolated_position =  lerp_vec3(current_frame.position, next_frame.position, progression);
  quat interpolated_rotation = slerp_quat(current_frame.rotation, next_frame.rotation, progression);

  mat4 local_matrix;
  imat4(local_matrix);
  
  mat4 rotation_matrix;
  mat4_from_quat(rotation_matrix, interpolated_rotation);
  mat_multiply(local_matrix, rotation_matrix);
  mat_translate(local_matrix, interpolated_position);
  
  memcpy(out_transform, local_matrix, sizeof(mat4));
}

internal void
update_animated_joint_array(animation* animation, r32 dt, joint* current_joint, r32* parent_transform) {
  // @temporary
  joint_animation* joint_anim_data = 0;
  for(u32 i = 0; i < animation->joint_animation_count; i++) {
    joint_animation* it = &animation->joint_animation_array[i];
    if(it->joint_id == current_joint->index) {
      joint_anim_data = it;
      break;
    }
  }

  mat4 final_current_transform;
  imat4(final_current_transform);

  if(joint_anim_data) {
    mat4 current_local_transform;
    get_joint_transform(current_local_transform, joint_anim_data, current_joint, dt); // @invert
    
    mat4 current_transform;
    mat_multiply(current_transform, parent_transform, current_local_transform);
    
    for(u32 i = 0; i < current_joint->child_count; i++) {
      joint* it = &current_joint->child_array[i];
      update_animated_joint_array(animation, dt, it, current_transform);
    }
    
    mat_multiply(final_current_transform, current_transform, current_joint->inverse_bind_transform);
  }
  
  memcpy(current_joint->animated_transform, final_current_transform, sizeof(mat4));
}

internal void
animation_update(animation* animation, joint* root_joint, r32 dt) {
  mat4 imat;
  imat4(imat);
  
  update_animated_joint_array(animation, dt, root_joint, imat);
}

#if 0
internal joint*
get_joint_by_id(joint* parent, u32 index) {
  joint* result = 0;
  if(parent->index == index) {
    result = parent;
  } else {
    for(u32 i = 0; i < parent->child_count; i++) {
      joint* ijoint = parent->child_array[i];
      result = get_joint_by_id(ijoint, index);
      if(result) {
	break;
      }
    }
  }
  
  return(result);
}

internal void
update_animated_joint_array(mesh_anim* mesh_anim, animation* animation, r32 dt) {
  for(u32 i = 0; i < animation->joint_animation_count; i++) {
    joint_animation* it = &animation->joint_animation_array[i];
    joint* joint = get_joint_by_id(mesh_anim->root_joint, it->joint_id);
    
    mat4 current_local_transform;
    mat4 parent_transform;
    
    get_joint_transform(current_local_transform, it, joint, dt);
    imat4(parent_transform);

    if(joint->parent) {
      get_joint_transform(parent_transform, it, joint->parent, dt);
    }
    
    mat4 current_transform;
    mat_multiply(current_transform, parent_transform, current_local_transform);

    mat4 final_transform;
    mat_multiply(final_transform, current_transform, joint->inverse_bind_transform);
    memcpy(joint->animated_transform, final_transform, sizeof(mat4));
  }  
}
#endif

