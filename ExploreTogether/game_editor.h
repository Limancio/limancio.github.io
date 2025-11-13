#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#include "graphics/gui.h"
#include "graphics/mesh_loader.h"
#include "game.h"

#define DEFAULT_CUBE_SIZE     	 0.25f
#define GIZMO_CUBE_SIZE     	 0.05f

#define GIZMO_ARROW_BODY_RADIUS  0.025f
#define GIZMO_ARROW_BODY_HEIGHT  0.5f
#define GIZMO_ARROW_HEAD_RADIUS  0.045f
#define GIZMO_ARROW_TOTAL_HEIGHT 0.65f
#define GIZMO_SPHERE_RADIUS      1.0f

#define GAME_OBJECT_SELECT_MAX  50

#define GIZMO_DRAW_NONE_AXIS 0x1
#define GIZMO_DRAW_X_AXIS    0x2
#define GIZMO_DRAW_Y_AXIS    0x4
#define GIZMO_DRAW_Z_AXIS    0x8
#define GIZMO_DRAW_XZ_AXIS   0x10
#define GIZMO_DRAW_YZ_AXIS   0x20
#define GIZMO_DRAW_XY_AXIS   0x40
#define GIZMO_DRAW_ALL_AXIS  GIZMO_DRAW_X_AXIS | GIZMO_DRAW_Y_AXIS | GIZMO_DRAW_Z_AXIS | GIZMO_DRAW_XZ_AXIS | GIZMO_DRAW_YZ_AXIS | GIZMO_DRAW_XY_AXIS  

#define EDIT_POSITION 0x1
#define EDIT_SCALE    0x2
#define EDIT_ROTATION 0x4
#define EDIT_MESH     0x8
            
#define PANEL_BASIC    0x1
#define PANEL_MATERIAL 0x2
#define PANEL_FLAG     0x4
#define PANEL_SQUARE_INFO 0x10

struct gizmo_animation_style {
  vec4 color 	    = {0.65f, 0.65f, 0.65f, 1.0f};
  vec4 hot_color    = {0.85f, 0.85f, 0.85f, 1.0f};
  vec4 active_color = {0.99f, 0.99f, 0.99f, 1.0f};
};

struct gizmo_style {
  vec4 x_axis_color = {0.95f, 0.00f, 0.00f, 1.0f};
  vec4 y_axis_color = {0.00f, 0.95f, 0.00f, 1.0f};
  vec4 z_axis_color = {0.00f, 0.00f, 0.95f, 1.0f};
  vec4 scale_color  = {1.0f, 1.0f, 0.0f, 1.0f};
  vec4 triangle_color = {1.0f, 1.0f, 0.0f, 1.0f};

  // used for ray casting ID colors
  vec4 xz_plane_color = {1.0f, 0.0f, 1.0f, 1.0f};
  vec4 xy_plane_color = {1.0f, 1.0f, 0.0f, 1.0f};
  vec4 yz_plane_color = {0.0f, 1.0f, 1.0f, 1.0f};
};

enum gizmo_type { TRANSLATE = 0, ROTATE, SCALE };

// editor gizmo state flags
const u32 GIZMO_HOT    	 = 0x1;
const u32 GIZMO_ACTIVE 	 = 0x2;
const u32 GIZMO_X_AXIS   = 0x4;
const u32 GIZMO_Y_AXIS   = 0x8;
const u32 GIZMO_Z_AXIS   = 0x10;
const u32 GIZMO_XZ_AXIS  = 0x20;
const u32 GIZMO_YZ_AXIS  = 0x40;
const u32 GIZMO_XY_AXIS	 = 0x80;
const u32 GIZMO_ALL_AXIS = 0x100;

enum editor_tool   { EDIT_ENTITY = 0, EDIT_DISPLACEMENT, EDIT_OCEAN, MESH_INSTANCE_PANEL };
enum entity_tool   { EDIT_BASICS = 0, EDIT_FLAGS };
enum editor_action { NO_ACTION, SELECT, MULTI_SELECT };
enum displacement_tool { EDIT_INSTANCE = 0, EDIT_HEIGHT, EDIT_TEXTURE };

struct entity_editor {
  u32 entity_selected_array[MAX_ENTITY_COUNT];
  u32 entity_selected_count;
    
  entity_tool tool;
  gizmo_type gizmo_editor_type = TRANSLATE;
};

struct displacement_editor {
  displacement_tool tool;
  
  value_index selection_index;
  
  temporary_pool* sample_pool; // @temporary: dynamic array 
  sample_data* active_sample;

  value_index select_array[MAX_DISPLACEMENT_SELECT];
  u32 select_count;
};

enum gizmo_view_angle {
  MIN_X_MIN_Z = 0,
  MIN_X_MAX_Z,
  MAX_X_MIN_Z,
  MAX_X_MAX_Z
};

struct gizmo_data {
  u32 state;
  u32 last_state;
  
  vec3 plane_normal;
  vec3 origin_position;
  
  vec3 cursor_to_origin_position;
  r32  cursor_to_origin_distance;
  
  vec3 delta_position;
  quat delta_rotation;
  vec3 delta_scale;
};

enum recovery_type {DO = 0, UNDO, REDO};

struct recovery_handler {
  dormammu_buffer temp_buffer;
  dormammu_buffer buffer;
  
  size_t buffer_cursor;
  size_t changes_cursor;
  
  size_t changes_count;
  b32 can_undo;
  b32 can_redo;
};

/*
struct mesh_viewer_render {
  mesh_type type;
  
  mesh_render mesh_render;
  mesh_anim_render mesh_anim_render;
};

struct mesh_viewer {
  mat4 mesh_proj_matrix;
  radius_camera mesh_camera;
  
  renderer_buffer_data mesh_viewer_buffer_data;
  mesh_viewer_render mesh_array[1000]; // @temporary
  u32 mesh_count;
}
*/

struct game_editor {
  recovery_handler recovery_handler;
  
  mesh_file_data mesh_loader_data;
  radius_camera mesh_camera;
  mat4 mesh_proj_matrix;
  
  // settings
  editor_tool tool;
  entity_editor entity_editor;
  displacement_editor displacement_editor;
    
  // @temporary: ui
  u32 panel_active_flags;
  u32 material_active_index;
  
  game_level* active_level;
  b32 show_editor;
  b32 show_wireframe;
  b32 mesh_catalog_active;
  b32 level_draw_water;
  
  vec3 cursor_ray;
  
  b32 ignore_cursor;
  b32 ignore_keyboard;
  
  // utility gizmos values
  gizmo_data gizmo_data;
  
  catalog_handler* catalog_handler;
  shader_program* scene_shader;
  shader_program* editor_shader;
  r64 select_elapsed;
  
  renderer_buffer_data mesh_viewer_buffers;
  renderer_draw_data   editor_render_data;
  renderer_buffer_data editor_buffers;
  renderer_buffer_data scene_buffers;
  
  temporary_pool* gizmo_data_pool;
  gizmo_mesh gizmo_cube;
  gizmo_mesh gizmo_arrow_body;
  gizmo_mesh gizmo_arrow_head;
  gizmo_mesh gizmo_arrow_scale_head;
  gizmo_mesh gizmo_sphere;
  gizmo_mesh gizmo_circle;
  gizmo_mesh gizmo_torus;
  gizmo_mesh gizmo_rotate_cursor;
  
  mat4 gizmo_xaxis_matrix;
  mat4 gizmo_yaxis_matrix;
  mat4 gizmo_zaxis_matrix;
};

#endif // !GAME_EDITOR_H
