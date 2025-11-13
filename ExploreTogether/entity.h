#ifndef ENTITY_H
#define ENTITY_H

#include "graphics/mesh.h"

enum entity_type {STATIC_OBJECT = 0, DYNAMIC_OBJECT, PLAYER_OBJECT, SQUARE_INFO_OBJECT};

char *entity_type_names[] = {
  "STATIC_OBJECT",
  "DYNAMIC_OBJECT",
  "PLAYER_OBJECT",
  "SQUARE_INFO_OBJECT"
};

struct entity {
  void* object_pointer;
  entity_type type; @parse(U32)
  u32 id; @parse(U32)
  
  vec3 position = {0.0f, 0.0f, 0.0f}; @parse(VEC3)
  quat rotation = {0.0f, 0.0f, 0.0f, 1.0f}; @parse(QUAT)
  vec3 scale = {1.0f, 1.0f, 1.0f}; @parse(VEC3)
  
  u32 flags; @parse(U32)
  static_string name; @parse(STRING)
  static_string target_mesh_name; @parse(STRING)
  
  mesh_render mesh_render; 
  mat4 model_matrix;
  
  material_render* material_array[MAX_MATERIAL_PER_MESH];
  u32 material_count; 
};

struct static_object {
  u32 entity_id;
};

struct dynamic_object {
  u32 entity_id;
};

#define MAX_SQUARE_LINK 9
struct square_object {
  u32 entity_id;
  
  u32 square_link_array[MAX_SQUARE_LINK]; // @note: Refeers to entity ids.
  u32 square_link_count;
};

struct player {
  u32 entity_id;

  r32 delta        = 0.0f;
  r32 target_delta = 0.0f;

  r32 move_speed = 1.5f;
};

// @temporary
inline internal void
update_model_matrix(entity* entity) {
  mat4 model_matrix;
  imat4(model_matrix);

  mat4 rotation_matrix;
  mat4_from_quat(rotation_matrix, entity->rotation);
  
  mat_scale(model_matrix, entity->scale);
  mat_multiply(model_matrix, rotation_matrix);
  mat_translate(model_matrix, entity->position);
  
  memcpy(entity->model_matrix, model_matrix, sizeof(mat4));
}

/*internal void
clear_mesh_from_entity(mesh_catalog* mesh_catalog, texture_catalog* texture_catalog, string_arena* string_arena, renderer* renderer, entity* entity) {
  try_clear_mesh_from_bucket(mesh_catalog, texture_catalog, string_arena, renderer, entity->mesh, entity->type);

  for(u32 i = 0; i < entity->material_count; i++) {
    entity->material_array[i] = {};
  }
  
  entity->mesh = NULL;
  entity->material_count = 0;
}*/

inline internal void
copy_entity_values(entity* out, entity* entity) {
#if 0 // @incomplete:
  out->flags    = entity->flags;
  out->type     = entity->type;
  out->name     = entity->name; // @incomplete: use the same pointer adress
  out->position = entity->position;
  out->rotation = entity->rotation;
  out->scale    = entity->scale;
  memcpy(out->model_matrix, entity->model_matrix, sizeof(mat4));

  out->mesh     = entity->mesh;
  out->material_count = entity->material_count;
  for(u32 i = 0; i < out->material_count; i++) {
    out->material_array[i] = entity->material_array[i];
  }
#endif
}
#if 0
inline internal void
remove_link_info(square_object* object, u32 delete_index) {
  if(object->square_link_count > 1) {
    u32 remainder_count = object->square_link_count - delete_index;
    memcpy(&object->square_link_array[delete_index], &object->square_link_array[delete_index + 1], sizeof(u32) * remainder_count);
  }
  object->square_link_count -= 1;
  if(object->square_link_count == 0) { memset(object->square_link_array, 0, sizeof(u32) * MAX_SQUARE_LINK); }
}

internal void
delete_link_to_square_object(square_object* object, square_object* link_object) {
  u32 delete_object_index, delete_link_object_index;
  
  for(u32 index = 0; index < object->square_link_count; index++) {
    if(object->square_link_array[index] == link_object->entity_id) {
      delete_object_index = index;
    }
  }
  for(u32 index = 0; index < link_object->square_link_count; index++) {
    if(link_object->square_link_array[index] == object->entity_id) {
      delete_link_object_index = index;
    }
  }
  
  remove_link_info(object, delete_object_index);
  remove_link_info(link_object, delete_link_object_index);
}

inline internal void
remove_all_link_info(game_level* level, square_object* object) {
  u32 link_array[MAX_SQUARE_LINK];
  u32 link_count = object->square_link_count;
  
  for(u32 i = 0; i < link_count; i++) {
    link_array[i] = object->square_link_array[i];
  }
  
  for(u32 i = 0; i < link_count; i++) {
    entity* entity_pointer = get_entity_by_id(level, link_array[i]);
    square_object* link_object = static_cast<square_object*>(entity_pointer->object_pointer);
    delete_link_to_square_object(object, link_object);
  }
}

internal void
add_link_to_square_object(square_object* object, square_object* link_object) {
  if(object->square_link_count < MAX_SQUARE_LINK && link_object->square_link_count < MAX_SQUARE_LINK) {
    u32 object_index = object->square_link_count;
    object->square_link_array[object_index] = link_object->entity_id;
    object->square_link_count = object->square_link_count + 1;
    
    u32 link_index = link_object->square_link_count;
    link_object->square_link_array[link_index] = object->entity_id;
    link_object->square_link_count = link_object->square_link_count + 1;
  }
}
#endif

#endif // !ENTITY_H
