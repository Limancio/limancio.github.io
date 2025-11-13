#ifndef GAME_H
#define GAME_H

#include "graphics/game_camera.h"
#include "window/game_window.h"

#include "entity.h"
#include "displacement_entity.h"

// @temporary:
#define GAME_OBJECT_COUNT  128
#define MAX_ENTITY_COUNT   128  

struct ocean {
  b32 active;
  texture* dudv_texture;
  r32 water_size = 25.0f;
  r32 tile_size  = 1.0f;
  
  r32 water_move_elapsed = 0.0f;
  r32 water_height 	 = 1.0f;
  
  vec4 water_color       = {0.0f, 0.3f, 0.6f, 1.0f};
  r32 clearness		 = 0.2f;
  r32 border_alpha_power = 5.0f;
  r32 tile_bias 	 = 0.75f;
  r32 wave_strength 	 = 0.02f;
  r32 wave_speed 	 = 0.03f;
  r32 refractivity       = 1.0f;

  r32 wave_length        = 10.0f;
  r32 wave_amplitude     = 0.2f;

  vec2 wave1_force_direction = {1.0f, 0.0f}; @ignore_field
  vec2 wave2_force_direction = {0.5f, 0.0f}; @ignore_field
};

struct entity_hash {
  u32 key;
  entity value;
};

struct displacement_hash {
  u32 key;
  displacement value;
};

struct game_level {
  string level_name;
  game_camera camera;
  
  b32 player_on_level;
  player player;
  
  square_object square_object_array[GAME_OBJECT_COUNT];
  u32 square_object_count;
  
  static_object static_object_array[GAME_OBJECT_COUNT];
  u32 static_object_count;
  
  dynamic_object dynamic_object_array[GAME_OBJECT_COUNT];
  u32 dynamic_object_count;
  
  // @temporary: usage of stb_ds hash tables
  displacement_hash* displacement_array;
  
  // @temporary: usage of stb_ds hash tables
  u32 next_entity_id; // @incomplete: increment this value
  entity_hash* entity_array;
  
  ocean ocean_data;
  mesh_render ocean_mesh;
  mesh_render sky_mesh;
};

struct game_state {
  b32 exit;
  game_level level;
  
  // cached data
  mat4 ortho_matrix;
  mat4 persp_matrix;
  mat4 view_matrix;
  mat4 model_matrix;
};

internal displacement*
get_displacement_by_id(game_level* level, u32 id) {
  displacement* result = NULL;
  ptrdiff_t displacement_index = hmgeti(level->displacement_array, id);

  if(displacement_index >= 0) {
    result = &level->displacement_array[displacement_index].value;
  }
  
  return(result);
}

internal entity*
get_entity_by_id(game_level* level, u32 id) {
  entity* result = NULL;
  ptrdiff_t entity_index = hmgeti(level->entity_array, id);

  if(entity_index >= 0) {
    result = &level->entity_array[entity_index].value;
  }
  
  return(result);
}

#endif // !GAME_H
