#ifndef DISPLACEMENT_ENTITY_H
#define DISPLACEMENT_ENTITY_H

struct displacement_style {
  vec4 selection_color = {0.05f, 0.05f, 0.1f,  1.0f};
  vec4 instanced_color = {0.05f, 0.01f, 0.05f, 1.0f};
  vec4 cursor_color    = {0.25f, 0.25f, 0.25f, 1.0f};
  vec4 delete_color    = {0.1f,  0.05f, 0.05f, 1.0f};
};

union value_index {
  u32 value;
  s16 data[2];
};

bool operator==(value_index a, value_index b) {
  return(a.data[0] == b.data[0] && a.data[1] == b.data[1]);
}

struct displacement {
  value_index index;
  vec3 position;
  mesh_render mesh_render;
  
  material_render* material_array[MAX_MATERIAL_PER_MESH];
  u32 material_count; 
};

struct sample_data {
  u32  index;
  vec3 position;
};

inline internal value_index
get_index_at(r32 position_x, r32 position_z, u32 x, u32 z) {
  value_index value;
  
  value.data[0] = static_cast<u16>(position_x + static_cast<r32>(x) * DISPLACEMENT_VERTEX_SIZE);
  value.data[1] = static_cast<u16>(position_z + static_cast<r32>(z) * DISPLACEMENT_VERTEX_SIZE);
  return(value);
}

inline internal sample_data*
get_sample_data(temporary_pool* sample_bucket, u32 vertex_index) {
  sample_data* result = 0;
  
  {
    while(sample_bucket) {
      sample_data* sample_array = reinterpret_cast<sample_data*>(sample_bucket->buffer);
      size_t max_sample_count = (sample_bucket->buffer_size / sizeof(sample_data));
      
      for(u32 i = 0; i < max_sample_count; i++) {
	sample_data* it = &sample_array[i];
	if(it->index == vertex_index) {
	  result = it;
	  break;
	}
      }
	    
      sample_bucket = sample_bucket->next;
    }
  }
  
  return(result);
}

inline internal void
add_sample_data(temporary_pool*& sample_bucket, sample_data new_sample) {
  if(!get_sample_data(sample_bucket, new_sample.index)) {
    sample_data* sample = reinterpret_cast<sample_data*>(temporary_alloc(sample_bucket, sizeof(sample_data)));
    *sample = new_sample;
  }
}

#define SAMPLE_ROW_COUNT 5

//
// editor data:
//

#define MAX_DISPLACEMENT_SELECT  4096

struct kriging_data {
  r32 nugget = 0.0f;
  r32 sill   = 60.0f;
  r32 range  = 100.0f;

  temporary_pool* buffer_pool; // @temporary
  vec3* sample_array; //[SAMPLE_ROW_COUNT * SAMPLE_ROW_COUNT]; // @temporary
  u32 sample_count;
  
  r32* inverse_matrix;
  u32 row_count;
};

internal r32
calculate_kriging_height(kriging_data* data, vec3 target_position) {
  r32 result = 0;

  r32* weight_values_array = static_cast<r32*>(temporary_alloc(data->buffer_pool, sizeof(r32) * data->row_count));
  vec2 v2_target_position = {target_position.x, target_position.z};
  
  for(u32 i = 0; i < data->row_count - 1; i++) {
    vec2 v2_sample = {data->sample_array[i].x, data->sample_array[i].z};
    
    r32 h = distance(v2_target_position, v2_sample);
    weight_values_array[i] = calculate_spherical_cov(h, data->nugget, data->sill, data->range);
  }
  weight_values_array[data->row_count - 1] = 1.0f;
  
  size_t matrix_size = sizeof(r32) * data->row_count;
  r32* matrix = static_cast<r32*>(temporary_alloc(data->buffer_pool, matrix_size));
  mat_multiply(matrix, data->inverse_matrix, weight_values_array, data->row_count, 1);
  
  for(u32 i = 0; i < data->row_count - 1; i++) {
    result += matrix[i] * data->sample_array[i].y;
  }
  
  return(result);
}

#endif // !DISPLACEMENT_ENTITY_H
