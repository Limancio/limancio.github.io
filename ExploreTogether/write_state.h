#ifndef WRITE_STATE_H
#define WRITE_STATE_H

internal displacement*
read_displacement_state(game_level* game_level, mesh_catalog* mesh_catalog, texture_catalog* texture_catalog, string path) {
  hot_file file = {};
  string_arena* string_arena = &file.string_arena;
  displacement* new_displacement = 0;
  
  read_hot_file(&file, path);
  
  u32 u32_index;
  read_value(&file, &u32_index, const_string("index"));
  
  value_index displacement_index = {u32_index};
  new_displacement = instantiate_displacement(game_level, u32_index);
  
  vec3 displacement_position;
  u32 vertex_count;
  u32 texture_count;
  
  read_value(&file, &displacement_position, const_string("position"));
  read_value(&file, &vertex_count, const_string("vertex_count"));
  read_value(&file, &texture_count, const_string("texture_count"));
  
  new_displacement->index = displacement_index;
  new_displacement->position = displacement_position;
  
  displacement_mesh* mesh_data = &new_displacement->mesh_data;
  mesh_data->vertex_count  = vertex_count;
  mesh_data->indices_count = vertex_count * 6;
  mesh_data->texture_count = texture_count;
  
  init_mesh_displacement(mesh_catalog, new_displacement);
  read_value_array(&file, vertex_count, (void*&) mesh_catalog->height_array, const_string("height_array"));
  read_value_array(&file, vertex_count, (void*&) mesh_data->texture_index_array, const_string("texture_index_array"));
  
  u32 iheight   = 0;
  u32 iposition = 0;
  u32 itexture = 0;
  for(s32 z = 0; z < DISPLACEMENT_ROW_COUNT; z++) {
    for(s32 x = 0; x < DISPLACEMENT_ROW_COUNT; x++) {
      r32 height = mesh_catalog->height_array[iheight++];
      
      vec3 vertex_position = {displacement_position.x + (x * DISPLACEMENT_VERTEX_SIZE), 0.0f, displacement_position.z + (z * DISPLACEMENT_VERTEX_SIZE)};
      mesh_data->position_array[iposition++] = vertex_position.x;
      mesh_data->position_array[iposition++] = height;
      mesh_data->position_array[iposition++] = vertex_position.z;
      
      mesh_data->texture_index_array[itexture * 3] = mesh_data->texture_index_array[itexture * 3];
      mesh_data->texture_index_array[itexture * 3 + 1] = mesh_data->texture_index_array[itexture * 3 + 1];
      mesh_data->texture_index_array[itexture * 3 + 2] = mesh_data->texture_index_array[itexture * 3 + 2];

      itexture += 1;
    }
  }
  
  for(u32 i = 0; i < mesh_data->texture_count; i++) {
    string texture_path;
    read_value(&file, &texture_path, const_string("texture_path_array"));

    catalog_result result = get_texture_by_path(texture_catalog, string_arena, texture_path);
    mesh_data->texture_array[i] = result.texture;
  }
  
  clear_hot_file(&file);
  return(new_displacement);
}

internal void
write_and_clear_file(hot_file* file) {
  // @incomplete:
  temporary_pool*& file_data = file->data_pool;
  while(file_data) {
    size_t data_size = file_data->buffer_size - file_data->empty_size;
    write_append_file(write_path, file_data->buffer, data_size);
    file_data = file_data->next;
  }
  
  free_string_arena(&file->string_arena);
  clear_temporary_pool(file->data_pool);
}

internal void
write_displacement_state(displacement* displacement, string path) {
  hot_file file = {};
  string_arena* string_arena = &file.string_arena;
  
  u32 displacement_index = displacement->index.value;
  write_value(&file, U32,  &displacement_index,    const_string("index"));
  write_value(&file, VEC3, &displacement->position, const_string("position"));
  
  displacement_mesh* mesh_data = &displacement->mesh_data;
  write_value(&file, U32,  &mesh_data->vertex_count, const_string("vertex_count"));
  write_value(&file, U32, &mesh_data->texture_count, const_string("texture_count"));
  
  for(u32 i = 0; i < mesh_data->texture_count; i++) {
    texture* it = mesh_data->texture_array[i];
    write_value(&file, STRING, &it->path, const_string("texture_path_array"));
  }
  
  for(u32 i = 0; i < mesh_data->vertex_count; i++) {
    r32 height_value = mesh_data->position_array[i * 3 + 1];
    write_value(&file, R32, &height_value, const_string("height_array"));
  }
  
  for(u32 i = 0; i < mesh_data->vertex_count; i++) {
    r32 height_value = mesh_data->texture_index_array[i * 3 + 1];
    write_value(&file, R32, &height_value, const_string("texture_index_array"));
  }
  
  string write_path = alloc_string(string_arena, MAX_PATH);
  write_path = append_string(write_path, path);
  write_path = append_string(write_path, const_string("/"));
  write_path = append_string(write_path, u32_to_string(string_arena, displacement_index));
  write_path = append_string(write_path, const_string(".displacement"));
  
  // @incomplete:
  temporary_pool*& file_data = file.data_pool;
  while(file_data) {
    size_t data_size = file_data->buffer_size - file_data->empty_size;
    write_append_file(write_path, file_data->buffer, data_size);
    file_data = file_data->next;
  }
  
  free_string_arena(&file.string_arena);
  clear_temporary_pool(file.data_pool);
}

internal void
write_entity_state(entity* e, string path) {
  hot_file file = {};
  string_arena* string_arena = &file.string_arena;
  
#struct_parse_type entity e
  write_value(&file, struct_parse_field, &value_name->struct_field, const_string("struct_name"));
  
  string write_path = alloc_string(string_arena, MAX_PATH);
  write_path = append_string(write_path, path);
  write_path = append_string(write_path, const_string("/"));
  write_path = append_string(write_path, u32_to_string(string_arena, displacement_index));
  write_path = append_string(write_path, const_string(".displacement"));
  
  write_and_clear_file(&file);
}

internal void
save_game_state(game_state* state) {
  game_level* level = &state->level;
  string level_name = level->level_name;
  string_arena string_arena = {}; // @temporary
  
  string complete_path = string_concatenate(&string_arena, 3, level_directory_path, directory_slash_token, level_name);
  // delete_all_files_from_directory(&string_arena, complete_path); // @incomplete
  
  u32 displacement_count = hmlen(level->displacement_array);
  for(u32 i = 0; i < displacement_count; i++) {
    displacement* it = &level->displacement_array[i].value;
    write_displacement_state(it, complete_path);
  }
  
  // write_entities_state
}

internal void
load_game_state(game_state* state, renderer* renderer, mesh_catalog* mesh_catalog, texture_catalog* texture_catalog) {
  game_level* level = &state->level;
  string level_name = level->level_name;
  string_arena string_arena = {}; // @temporary
  
  string complete_path 	    = string_concatenate(&string_arena, 3, level_directory_path, directory_slash_token, level_name);
  string complete_open_path = string_concatenate(&string_arena, 2, complete_path, directory_open_token);
  
  { // read all displacements
    string file_type = const_string("displacement");

    u32 array_offset = 0;
    string file_name_array[1024] = {};
    u32 file_displacement_count  = get_file_count_from_directory(complete_open_path);
    read_all_files_from_directory(&string_arena, file_name_array, complete_open_path, array_offset, file_type);

    for(u32 i = 0; i < file_displacement_count; i++) {
      string displacement_path = string_concatenate(&string_arena, 3, complete_path, directory_slash_token, file_name_array[i]);
      
      displacement* new_displacement = read_displacement_state(level, mesh_catalog, texture_catalog, displacement_path);
      displacement_mesh* mesh_data = &new_displacement->mesh_data;
      renderer_submit_displacement_mesh(renderer, mesh_catalog, mesh_data);
  
      free_string(&string_arena, &displacement_path);
      free_string(&string_arena, &file_name_array[i]);
    }
  }
  
  free_string(&string_arena, &complete_path);
  free_string(&string_arena, &complete_open_path);
}

#endif // !WRITE_STATE_H
