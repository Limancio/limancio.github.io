#include "game_editor.h"
#include "game.h"

#include "recovery_handler.h"
#include "graphics/mesh_creator.h"

#include "game_renderer.h"

internal void
init_game_editor(game_editor* editor, game_renderer* renderer, game_state* state, game_window* window, string_bucket* string_list, catalog_handler* catalog_handler) {
  // @temporary:
  editor->active_level = &state->level;
  editor->mesh_camera.radius = 1.0f;

  // @temporary: scene and render pointers:
  editor->catalog_handler = catalog_handler;
  editor->scene_shader    = renderer->scene_shader;
  editor->scene_buffers   = renderer->scene_buffer_data;
  editor->editor_buffers  = renderer->editor_buffer_data;

  // @temporary: init editor_render_data
  {
    renderer_draw_data* render_data = &editor->editor_render_data;
    u32 buffer_size = sizeof(ui_vertex) * QUAD_BUFFER_SIZE;
    
    glGenVertexArrays(1, &render_data->vao);
    glGenBuffers(1, &render_data->vbo);
  
    glBindVertexArray(render_data->vao);
    glBindBuffer(GL_ARRAY_BUFFER, render_data->vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, 0, GL_DYNAMIC_DRAW);
  
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (const GLvoid*) offsetof(ui_vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (const GLvoid*) offsetof(ui_vertex, uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (const GLvoid*) offsetof(ui_vertex, color));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    const u32 indices_count = 6 * QUAD_BUFFER_SIZE;
    GLuint indices_array[indices_count];
    u32 index = 0;
    for(u32 i = 0; i < QUAD_BUFFER_SIZE; i+=4) {
      indices_array[index++] = i;
      indices_array[index++] = i+1;
      indices_array[index++] = i+2;
      indices_array[index++] = i+2;
      indices_array[index++] = i+3;
      indices_array[index++] = i+1;
    }
  
    glGenBuffers(1, &render_data->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices_count, indices_array, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
    glBindVertexArray(0);
  }
  
  
  init_renderer_buffer_data(&editor->editor_buffers, window->iwidth, window->iheight);
  init_renderer_buffer_data(&editor->mesh_viewer_buffers, window->iwidth, window->iheight);
  editor->editor_shader = shader_catalog_add(catalog_handler->shader_catalog, string_list, const_string("data/shaders/editor.shader"), const_string("editor"));
  
  imat4(editor->gizmo_xaxis_matrix);
  imat4(editor->gizmo_yaxis_matrix);
  imat4(editor->gizmo_zaxis_matrix);
  mat_rotate(editor->gizmo_xaxis_matrix, 90.0f,  0.0f, 0.0f, 1.0f);
  mat_rotate(editor->gizmo_zaxis_matrix, 90.0f, -1.0f, 0.0f, 0.0f);
  
  mesh_data gizmo_mesh_data = {};
  create_gizmo_cube(&gizmo_mesh_data, GIZMO_CUBE_SIZE);
  renderer_submit_gizmo_mesh(&gizmo_mesh_data, &editor->gizmo_cube);
  
  create_gizmo_arrow_body(&gizmo_mesh_data, GIZMO_ARROW_BODY_RADIUS, GIZMO_ARROW_BODY_HEIGHT);
  renderer_submit_gizmo_mesh(&gizmo_mesh_data, &editor->gizmo_arrow_body);
  
  create_gizmo_arrow_head(&gizmo_mesh_data, GIZMO_ARROW_HEAD_RADIUS, GIZMO_ARROW_BODY_HEIGHT);
  renderer_submit_gizmo_mesh(&gizmo_mesh_data, &editor->gizmo_arrow_head);
  
  create_gizmo_cube(&gizmo_mesh_data, GIZMO_ARROW_HEAD_RADIUS, vec3{0, GIZMO_ARROW_BODY_HEIGHT, 0});
  renderer_submit_gizmo_mesh(&gizmo_mesh_data, &editor->gizmo_arrow_scale_head);
  
  create_gizmo_circumference(&gizmo_mesh_data, 0.45f, 0.5f, 36);
  renderer_submit_gizmo_mesh(&gizmo_mesh_data, &editor->gizmo_circle);
  
  create_gizmo_torus(&gizmo_mesh_data, 0.5f, GIZMO_ARROW_BODY_RADIUS, 32, 24);
  renderer_submit_gizmo_mesh(&gizmo_mesh_data, &editor->gizmo_torus);
  
  create_gizmo_torus(&gizmo_mesh_data, 0.5f, GIZMO_ARROW_BODY_RADIUS * 1.05f, 32, 24);
  renderer_submit_gizmo_mesh(&gizmo_mesh_data, &editor->gizmo_rotate_cursor);
  
  create_gizmo_sphere(&gizmo_mesh_data, 9, 18, GIZMO_SPHERE_RADIUS);
  renderer_submit_gizmo_mesh(&gizmo_mesh_data, &editor->gizmo_sphere);
}

internal void
get_mesh_by_name(catalog_handler* catalog_handler, mesh_file_data* file_data, string mesh_name) {
  mesh_render mesh = {};
  
  if(string_equals(mesh_name, const_string("gizmo_cube"))) {
    read_mesh_file(file_data, catalog_handler, const_string("data/models/gizmo/gizmo_cube.dae"));
  } else if(string_equals(mesh_name, const_string("gizmo_sphere"))) {
    read_mesh_file(file_data, catalog_handler, const_string("data/models/gizmo/gizmo_sphere.dae"));
  } else if(string_equals(mesh_name, const_string("gizmo_animation"))) {
    read_mesh_file(file_data, catalog_handler, const_string("data/models/gizmo/gizmo_animation.dae"));
  } else if(string_equals(mesh_name, const_string("gizmo_player"))) {
    read_mesh_file(file_data, catalog_handler, const_string("data/models/gizmo/gizmo_player.dae"));
  }
}

//
// mesh-viewer-draw:
//
internal void
draw_mesh_viewer(game_editor* game_editor, r32 dt) {
  shader_program* scene_shader = game_editor->scene_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_buffer_data* render_buffers = &game_editor->mesh_viewer_buffers;
  
  shader_set_framebuffer(render_buffers->frame_buffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0, 0, 1);
  shader_active(scene_shader);
  
  mat4 model_matrix;
  mat4 mesh_view_matrix;
  imat4(mesh_view_matrix);
  
  {
    radius_camera* mesh_cam = &game_editor->mesh_camera;
    mesh_cam->yaw += dt * 1.0f;
  
    vec3 target = mesh_cam->target;
    r32 radius  = mesh_cam->radius;
    r32 pitch   = mesh_cam->pitch;
    r32 yaw     = mesh_cam->yaw;
      
    mesh_cam->position.x = target.x + radius * cosf(pitch) * sinf(yaw);
    mesh_cam->position.y = target.y + radius * sinf(pitch);
    mesh_cam->position.z = target.z + radius * cosf(pitch) * cosf(yaw);
    mat_look_at(mesh_view_matrix, mesh_cam->position, mesh_cam->target, mesh_cam->up);
  }
      
  shader_set_matrix(catalog_handler->shader_catalog, scene_shader, const_string("proj_matrix"), game_editor->mesh_proj_matrix);
  shader_set_matrix(catalog_handler->shader_catalog, scene_shader, const_string("view_matrix"), mesh_view_matrix);

  mesh_file_data* file_data = &game_editor->mesh_loader_data;
  for(u32 i = 0; i < file_data->array_count; i++) {
    mesh_data* it = &file_data->mesh_data_array[i];
    
    imat4(model_matrix);
    shader_set_matrix(catalog_handler->shader_catalog, scene_shader, const_string("model_matrix"), model_matrix);
    
    // @incomplete!
  }
  
  shader_active(0);
  shader_set_framebuffer(0);
}

//
// gizmo-draw:
//
/*
// @incomplete: move to game_editor drawing stuff
  for(u32 i = 0; i < level->square_object_count; i++) {
    square_object* game_object = &level->square_object_array[i];
    entity* entity_data = get_entity_by_id(level, game_object->entity_id);    
    glUniformMatrix4fv(get_uniform_location(&renderer->scene_shader, "model_matrix"), 1, GL_FALSE, (const GLfloat*) entity_data->model_matrix);
    
    mesh* mesh_data = static_cast<mesh*>(entity_data->mesh);
    renderer_draw_mesh(renderer, mesh_data, false, entity_data->material_array, entity_data->material_count);
    
    mat4 identity_matrix;
    imat4(identity_matrix);
    glUniformMatrix4fv(get_uniform_location(&renderer->scene_shader, "model_matrix"), 1, GL_FALSE, (const GLfloat*) identity_matrix);
    
    r32 angle = to_radians(camera->pitch + 90.0f); // angle_between_vec3(camera->position, entity_data->position);
    quat gizmo_rotation = quat_from_euler({angle, 0.0f, 0.0f});
    
    for(u32 j = 0; j < game_object->square_link_count; j++) {
      entity* it = get_entity_by_id(level, game_object->square_link_array[j]);
      
      vec3 p0 = entity_data->position;
      vec3 p1 = it->position;

      renderer_draw_line(renderer, gizmo_rotation, p0, p1, 0.025f);
    }
  }

*/
internal r32
calculate_gizmo_size(vec3 gizmo_position, vec3 camera_position, r32 MIN_DISTANCE = 0.1f, r32 MAX_DISTANCE = 5.0f) {
  r32 gizmo_size = distance(gizmo_position, camera_position) / MAX_DISTANCE;
  gizmo_size = (gizmo_size < MIN_DISTANCE) ? MIN_DISTANCE : gizmo_size;
  return(gizmo_size);
}

internal void
draw_gizmo_plane(game_editor* game_editor, vec4 color, vec3 gizmo_position, u32 gizmo_plane_flags, gizmo_view_angle plane_view_angle, r32 scale = 1.0f) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_draw_data* render_data = &game_editor->editor_render_data;
  
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color);

  mat4 model_matrix;
  imat4(model_matrix);
  mat_scale(model_matrix, {scale, scale, scale});
  mat_translate(model_matrix, gizmo_position);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  
  const u32 count = 6;
  const r32 off   = GIZMO_ARROW_BODY_RADIUS * 0.5f;
  const r32 size  = 0.25f;
  
  if(gizmo_plane_flags & GIZMO_DRAW_XZ_AXIS) {
    switch(plane_view_angle) {
    case MIN_X_MIN_Z: {
      r32 vertex_array[] = { -off, 0.0f, -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, 0.0f, -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -off, 0.0f, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -off, 0.0f, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, 0.0f, -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, 0.0f, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_plane(render_data, vertex_array, count);
    } break;
    case MIN_X_MAX_Z: {
      r32 vertex_array[] = { -off, 0.0f, off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, 0.0f, off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -off, 0.0f, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -off, 0.0f, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, 0.0f, off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, 0.0f, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_plane(render_data, vertex_array, count);
    } break;
    case MAX_X_MIN_Z: {
      r32 vertex_array[] = { off, 0.0f, -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, 0.0f, -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     off, 0.0f, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     off, 0.0f, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, 0.0f, -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, 0.0f, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_plane(render_data, vertex_array, count);
    } break;
    case MAX_X_MAX_Z: {
      r32 vertex_array[] = { off, 0.0f, off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, 0.0f, off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     off, 0.0f, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     off, 0.0f, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, 0.0f, off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, 0.0f, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_plane(render_data, vertex_array, count);
    } break;
    }
  } else if(gizmo_plane_flags & GIZMO_DRAW_XY_AXIS) {
    switch(plane_view_angle) {
    case MIN_X_MIN_Z:
    case MIN_X_MAX_Z: {
      r32 vertex_array[] = { -off,  off, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, off, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -off, size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -off, size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, off, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     -size, size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_plane(render_data, vertex_array, count);
    } break;
    case MAX_X_MIN_Z:
    case MAX_X_MAX_Z: {
      r32 vertex_array[] = { off,  off, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, off, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     off, size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     off, size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, off, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     size, size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_plane(render_data, vertex_array, count);
    } break;
    }
  } else if(gizmo_plane_flags & GIZMO_DRAW_YZ_AXIS) {
    switch(plane_view_angle) {
    case MAX_X_MIN_Z:
    case MIN_X_MIN_Z: {
      r32 vertex_array[] = { 0.0f,  off,  -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f,  off, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f, size,  -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f, size,  -off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f,  off, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f, size, -size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_plane(render_data, vertex_array, count);
    } break;
    case MIN_X_MAX_Z:
    case MAX_X_MAX_Z: {
      r32 vertex_array[] = { 0.0f,  off,  off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f,  off, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f, size,  off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f, size,  off, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f,  off, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			     0.0f, size, size, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
      renderer_draw_plane(render_data, vertex_array, count);
    } break;
    }
  }  
}

internal void
draw_gizmo_arrow(game_editor* game_editor, vec4 color, vec3 gizmo_position, r32* gizmo_matrix, r32 scale = 1.0f) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color);
  
  mat4 model_matrix;
  memcpy(model_matrix, gizmo_matrix, sizeof(mat4));
  mat_scale(model_matrix, {scale, scale, scale});
  mat_translate(model_matrix, gizmo_position);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  
  renderer_draw_gizmo_mesh(&game_editor->gizmo_arrow_body);
  renderer_draw_gizmo_mesh(&game_editor->gizmo_arrow_head);
}

internal void
draw_gizmo_scale_arrow(game_editor* game_editor, vec4 color, vec3 gizmo_position, r32* gizmo_matrix, r32 scale = 1.0f, b32 draw_line = false) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color);
  
  mat4 model_matrix;
  memcpy(model_matrix, gizmo_matrix, sizeof(mat4));
  mat_scale(model_matrix, {scale, scale, scale});
  mat_translate(model_matrix, gizmo_position);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  
  renderer_draw_gizmo_mesh(&game_editor->gizmo_arrow_body);
  renderer_draw_gizmo_mesh(&game_editor->gizmo_arrow_scale_head);
}

internal void
draw_gizmo_scale_cube(game_editor* game_editor, vec4 color, vec3 gizmo_position, r32 scale = 1.0f) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;

  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color);
  
  mat4 model_matrix;
  imat4(model_matrix);
  mat_scale(model_matrix, {scale, scale, scale});
  mat_translate(model_matrix, gizmo_position);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  
  renderer_draw_gizmo_mesh(&game_editor->gizmo_cube); // @temporary @fix
}

internal void
draw_gizmo_sphere(game_editor* game_editor, vec4 color, vec3 gizmo_position, r32 scale = 1.0f) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color);
  
  mat4 model_matrix;
  imat4(model_matrix);
  mat_scale(model_matrix, {scale, scale, scale});
  mat_translate(model_matrix, gizmo_position);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  
  renderer_draw_gizmo_mesh(&game_editor->gizmo_sphere); // @temporary @fix
}

internal void
draw_gizmo_scale_circle(game_editor* game_editor, game_camera* camera, vec4 color, vec3 gizmo_position, r32 scale = 1.0f) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color);
  
  mat4 model_matrix;
  imat4(model_matrix);
  mat_rotate(model_matrix, camera->pitch + 90.0f, 1.0f, 0.0f, 0.0f);
  
  // @temporary @fix
  mat4 rotation_matrix;
  mat4_from_quat(rotation_matrix, quat_from_euler(vec3{0.0f, to_radians(camera->yaw), 0.0f}));
  mat_multiply(model_matrix, rotation_matrix);
  
  mat_scale(model_matrix, {scale, scale, scale});
  mat_translate(model_matrix, gizmo_position);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  
  renderer_draw_gizmo_mesh(&game_editor->gizmo_circle); 
}

internal void
draw_gizmo_circle_cursor(game_editor* game_editor, vec4 color, vec3 gizmo_position, quat gizmo_rotation, r32* gizmo_matrix, r32 scale = 1.0f) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;

  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color);
  
  mat4 model_matrix;
  memcpy(model_matrix, gizmo_matrix, sizeof(mat4));

  mat4 rotation_matrix;
  mat4_from_quat(rotation_matrix, gizmo_rotation);
  mat_multiply(model_matrix, rotation_matrix);

  mat_scale(model_matrix, {scale, scale, scale});
  mat_translate(model_matrix, gizmo_position);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  
  renderer_draw_gizmo_mesh(&game_editor->gizmo_rotate_cursor);  
}

internal void
draw_gizmo_circle(game_editor* game_editor, game_camera* camera, vec4 color, vec3 gizmo_position, r32* gizmo_matrix, r32 scale = 1.0f) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color);
  
  mat4 model_matrix;
  memcpy(model_matrix, gizmo_matrix, sizeof(mat4));
  mat_scale(model_matrix, {scale, scale, scale});
  mat_translate(model_matrix, gizmo_position);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);

  r32 draw_distance = distance(camera->position, gizmo_position);
  shader_set_valuef(catalog_handler->shader_catalog, shader, const_string("u_max_render_distance"), draw_distance);
  renderer_draw_gizmo_mesh(&game_editor->gizmo_torus);  
  shader_set_valuef(catalog_handler->shader_catalog, shader, const_string("u_max_render_distance"), 0);
}

internal vec3
gizmo_translate_editor(game_editor* game_editor, game_window* window, vec3 gizmo_position, game_camera* camera, mat4 view_matrix, r32 round_value = 0.0f, action_type action = action_type::ENTITY_TRANSLATE, u32 draw_axis_flags = GIZMO_DRAW_ALL_AXIS, u32 draw_plane_flags = GIZMO_DRAW_NONE_AXIS) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_draw_data* render_data  = &game_editor->editor_render_data;
  
  vec3 move_position   = {};
  vec3 camera_position = camera->position;
  r32 gizmo_size       = calculate_gizmo_size(gizmo_position, camera_position);
  
  gizmo_view_angle view_plane_angle;
  if(draw_axis_flags) {
    vec2 gizmo_origin   = { gizmo_position.x,  gizmo_position.z};
    vec2 camera_pos     = {camera_position.x, camera_position.z};
    vec2 delta_position = camera_pos - gizmo_origin;
    
    if(delta_position.x <= 0 && delta_position.y <= 0) {
      view_plane_angle = MIN_X_MIN_Z;
    } else if(delta_position.x >= 0 && delta_position.y <= 0) {
      view_plane_angle = MAX_X_MIN_Z;
    } else if(delta_position.x <= 0 && delta_position.y >= 0) {
      view_plane_angle = MIN_X_MAX_Z;
    } else if(delta_position.x >= 0 && delta_position.y >= 0) {
      view_plane_angle = MAX_X_MAX_Z;
    }
  }
  
  gizmo_animation_style animation;
  gizmo_style default_style;
  gizmo_style style;
  GLubyte gizmo_active_info[3];
  
  {
    renderer_buffer_data* editor_buffers = &game_editor->editor_buffers;
    shader_set_framebuffer(editor_buffers->frame_buffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_active(shader);  
    
    if(draw_axis_flags & GIZMO_DRAW_X_AXIS) {
      draw_gizmo_arrow(game_editor, style.x_axis_color, gizmo_position, game_editor->gizmo_xaxis_matrix, gizmo_size);
    }
    if(draw_axis_flags & GIZMO_DRAW_Y_AXIS) {
      draw_gizmo_arrow(game_editor, style.y_axis_color, gizmo_position, game_editor->gizmo_yaxis_matrix, gizmo_size);
    }
    if(draw_axis_flags & GIZMO_DRAW_Z_AXIS) {
      draw_gizmo_arrow(game_editor, style.z_axis_color, gizmo_position, game_editor->gizmo_zaxis_matrix, gizmo_size);
    }

    if(draw_axis_flags & GIZMO_DRAW_XZ_AXIS) {
      draw_gizmo_plane(game_editor, style.xz_plane_color, gizmo_position, GIZMO_DRAW_XZ_AXIS, view_plane_angle, gizmo_size);
    }
    if(draw_axis_flags & GIZMO_DRAW_YZ_AXIS) {
      draw_gizmo_plane(game_editor, style.yz_plane_color, gizmo_position, GIZMO_DRAW_YZ_AXIS, view_plane_angle, gizmo_size);
    }
    if(draw_axis_flags & GIZMO_DRAW_XY_AXIS) {
      draw_gizmo_plane(game_editor, style.xy_plane_color, gizmo_position, GIZMO_DRAW_XY_AXIS, view_plane_angle, gizmo_size);
    }
      
    shader_active(0);
    get_color_from_buffer(gizmo_active_info, (s32) window->mousex, (s32) window->mousey);
    shader_set_framebuffer(0);
  }
  
  gizmo_data* gizmo_data   = &game_editor->gizmo_data;
  b32 	      gizmo_active = (gizmo_data->state & GIZMO_ACTIVE);
  
  if(gizmo_active) {
    if(window->mouse_button_left_state & MOUSE_BUTTON_LEFT_UP) {
      gizmo_data->state = 0;
      gizmo_active = false;
    }
  } else {
    if(gizmo_active_info[0] || gizmo_active_info[1] || gizmo_active_info[2]) {
      vec3 plane_normal;
      gizmo_data->state = (gizmo_active) ? (GIZMO_HOT | GIZMO_ACTIVE) : GIZMO_HOT;
      
      if(gizmo_active_info[0] && gizmo_active_info[2]) {
	gizmo_data->state |= GIZMO_XZ_AXIS;
	plane_normal = {0.0f, 1.0f, 0.0f};
      } else if(gizmo_active_info[0] && gizmo_active_info[1]) {
	gizmo_data->state |= GIZMO_XY_AXIS;
	plane_normal = {0.0f, 0.0f, 1.0f};
      } else if(gizmo_active_info[1] && gizmo_active_info[2]) {
	gizmo_data->state |= GIZMO_YZ_AXIS;
	plane_normal = {1.0f, 0.0f, 0.0f};
      } else if(gizmo_active_info[0] != 0) {
	gizmo_data->state |= GIZMO_X_AXIS;
	plane_normal = {0.0f, 1.0f, 0.0f};
      } else if(gizmo_active_info[1] != 0) {
	gizmo_data->state |= GIZMO_Y_AXIS;
	plane_normal = {view_matrix[2], view_matrix[6], view_matrix[10]};
      } else if(gizmo_active_info[2] != 0) {
	gizmo_data->state |= GIZMO_Z_AXIS;
	plane_normal = {0.0f, 1.0f, 0.0f};
      }

      b32 do_active = (window->mouse_button_left_state & MOUSE_BUTTON_LEFT_DOWN) && game_editor->ignore_cursor == false;
      if(do_active) {
	write_action(game_editor, action, recovery_type::DO);
	gizmo_data->state     	   |= GIZMO_ACTIVE;
	gizmo_data->plane_normal    = plane_normal;
	gizmo_data->origin_position = hit_plane(game_editor->cursor_ray, camera_position, gizmo_data->plane_normal, gizmo_position);
	gizmo_data->cursor_to_origin_position = (gizmo_position - gizmo_data->origin_position);
	
	gizmo_data->delta_position = gizmo_data->origin_position;
      }
    } else {
      gizmo_data->state &= ~GIZMO_HOT;
    }
  }
  
  vec3 cursor_hit_point = {};
  if(gizmo_active) {
    cursor_hit_point = hit_plane(game_editor->cursor_ray, camera_position, gizmo_data->plane_normal, gizmo_position);
    move_position = cursor_hit_point - gizmo_data->delta_position;
    
    if(gizmo_data->state & GIZMO_XZ_AXIS) {
      move_position.y = 0.0f;
    } else if(gizmo_data->state & GIZMO_XY_AXIS) {
      move_position.z = 0.0f;
    } else if(gizmo_data->state & GIZMO_YZ_AXIS) {
      move_position.x = 0.0f;
    } else if(gizmo_data->state & GIZMO_X_AXIS) {
      move_position = {move_position.x, 0.0f, 0.0f};
    } else if(gizmo_data->state & GIZMO_Y_AXIS) {
      move_position = {0.0f, move_position.y, 0.0f};
    } else if(gizmo_data->state & GIZMO_Z_AXIS) {
      move_position = {0.0f, 0.0f, move_position.z};
    }
    
    gizmo_data->delta_position = cursor_hit_point;
  }
  
  {
    renderer_buffer_data* scene_buffers = &game_editor->scene_buffers;
    shader_set_framebuffer(scene_buffers->frame_buffer);
    shader_active(shader);
    
    if(!gizmo_active) {
      shader_disable(GL_DEPTH_TEST);

      if(draw_axis_flags & GIZMO_DRAW_X_AXIS) {
	draw_gizmo_arrow(game_editor, style.x_axis_color, gizmo_position, game_editor->gizmo_xaxis_matrix, gizmo_size);
      }
      if(draw_axis_flags & GIZMO_DRAW_Y_AXIS) {
	draw_gizmo_arrow(game_editor, style.y_axis_color, gizmo_position, game_editor->gizmo_yaxis_matrix, gizmo_size);
      }
      if(draw_axis_flags & GIZMO_DRAW_Z_AXIS) {
	draw_gizmo_arrow(game_editor, style.z_axis_color, gizmo_position, game_editor->gizmo_zaxis_matrix, gizmo_size);
      }
      
      if(draw_axis_flags & GIZMO_DRAW_XZ_AXIS) {
	draw_gizmo_plane(game_editor, style.y_axis_color, gizmo_position, GIZMO_DRAW_XZ_AXIS, view_plane_angle, gizmo_size);
      }
      if(draw_axis_flags & GIZMO_DRAW_YZ_AXIS) {
	draw_gizmo_plane(game_editor, style.x_axis_color, gizmo_position, GIZMO_DRAW_YZ_AXIS, view_plane_angle, gizmo_size);
      }
      if(draw_axis_flags & GIZMO_DRAW_XY_AXIS) {
	draw_gizmo_plane(game_editor, style.z_axis_color, gizmo_position, GIZMO_DRAW_XY_AXIS, view_plane_angle, gizmo_size);
      }
      
      shader_enable(GL_DEPTH_TEST);
    }
    
    
    if(gizmo_active) {      
      mat4 model_matrix;

      // draw line gizmo:
      {
	vec3 min = { 0.0f, -100.0f, 0.0f };
	vec3 max = { 0.0f,  100.0f, 0.0f };
      
	if(gizmo_data->state & GIZMO_X_AXIS) {
	  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), style.x_axis_color); 
	  memcpy(model_matrix, game_editor->gizmo_xaxis_matrix, sizeof(mat4));
	}
	if(gizmo_data->state & GIZMO_Y_AXIS) {
	  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), style.y_axis_color); 
	  memcpy(model_matrix, game_editor->gizmo_yaxis_matrix, sizeof(mat4));
	}
	if(gizmo_data->state & GIZMO_Z_AXIS) {
	  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), style.z_axis_color); 
	  memcpy(model_matrix, game_editor->gizmo_zaxis_matrix, sizeof(mat4));
	}

	if(gizmo_data->state & GIZMO_X_AXIS || gizmo_data->state & GIZMO_Y_AXIS || gizmo_data->state & GIZMO_Z_AXIS) { // @temporary
	  mat_translate(model_matrix, gizmo_position);
	  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
	  renderer_draw_line(render_data, min, max);
	}
      }
      
      imat4(model_matrix);
      shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
      shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), style.triangle_color); 
      
      vec3 c0, c1, c2;
      if(gizmo_data->state & GIZMO_XZ_AXIS) {
	c0 = {gizmo_data->origin_position.x, cursor_hit_point.y, gizmo_data->origin_position.z};
	c1 = {cursor_hit_point.x, 	     cursor_hit_point.y, gizmo_data->origin_position.z};
	c2 = {cursor_hit_point.x, 	     cursor_hit_point.y, 	    cursor_hit_point.z};
      } else if(gizmo_data->state & GIZMO_XY_AXIS) {
	c0 = {gizmo_data->origin_position.x, gizmo_data->origin_position.y, gizmo_data->origin_position.z};
	c1 = {gizmo_data->origin_position.x, cursor_hit_point.y, 	    gizmo_data->origin_position.z};
	c2 = {cursor_hit_point.x, 	     cursor_hit_point.y, 	    cursor_hit_point.z};
      } else if(gizmo_data->state & GIZMO_YZ_AXIS) {
	c0 = {gizmo_data->origin_position.x, gizmo_data->origin_position.y, gizmo_data->origin_position.z};
	c1 = {gizmo_data->origin_position.x, cursor_hit_point.y, 	    gizmo_data->origin_position.z};
	c2 = {cursor_hit_point.x, 	     cursor_hit_point.y, 	    cursor_hit_point.z};
      } else if(gizmo_data->state & GIZMO_X_AXIS) {
	c0 = {gizmo_data->origin_position.x, gizmo_data->origin_position.y, gizmo_data->origin_position.z};
	c1 = {cursor_hit_point.x, 	     gizmo_data->origin_position.y, gizmo_data->origin_position.z};
	c2 = {cursor_hit_point.x, 	     cursor_hit_point.y, 	    cursor_hit_point.z};
      } else if(gizmo_data->state & GIZMO_Y_AXIS) {
	c0 = {gizmo_data->origin_position.x, gizmo_data->origin_position.y, gizmo_data->origin_position.z};
	c1 = {gizmo_data->origin_position.x, cursor_hit_point.y, 	    gizmo_data->origin_position.z};
	c2 = {cursor_hit_point.x, 	     cursor_hit_point.y, 	    cursor_hit_point.z};
      } else if(gizmo_data->state & GIZMO_Z_AXIS) {
	c0 = {gizmo_data->origin_position.x, gizmo_data->origin_position.y, gizmo_data->origin_position.z};
	c1 = {gizmo_data->origin_position.x, gizmo_data->origin_position.y, cursor_hit_point.z};
	c2 = {cursor_hit_point.x, 	     cursor_hit_point.y, 	    cursor_hit_point.z};
      }
      
      renderer_draw_line(render_data, c0, c1);
      renderer_draw_line(render_data, c0, c2);
      renderer_draw_line(render_data, c1, c2);
    }
    
    shader_active(0);
    shader_set_framebuffer(0);
  }
  
  return(move_position);
}

internal vec3
gizmo_scale_editor(game_editor* game_editor, game_window* window, game_camera* camera, vec3 gizmo_position, mat4 proj_matrix, mat4 view_matrix, r32 round_value = 0.0f) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_draw_data* render_data = &game_editor->editor_render_data;
  
  vec3 move_scale      = {1.0f, 1.0f, 1.0f};
  vec3 camera_position = camera->position;
  r32 gizmo_size       = calculate_gizmo_size(gizmo_position, camera_position);
  
  gizmo_animation_style animation;
  gizmo_style style;
  gizmo_style default_style;
  GLubyte gizmo_active_info[3];
  
  {
    renderer_buffer_data* editor_buffers = &game_editor->editor_buffers;
    shader_set_framebuffer(editor_buffers->frame_buffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_active(shader);  
    
    draw_gizmo_scale_arrow(game_editor, style.x_axis_color, gizmo_position, game_editor->gizmo_xaxis_matrix, gizmo_size);
    draw_gizmo_scale_arrow(game_editor, style.y_axis_color, gizmo_position, game_editor->gizmo_yaxis_matrix, gizmo_size);
    draw_gizmo_scale_arrow(game_editor, style.z_axis_color, gizmo_position, game_editor->gizmo_zaxis_matrix, gizmo_size);
    draw_gizmo_scale_circle(game_editor, camera, style.scale_color, gizmo_position, gizmo_size);
    
    get_color_from_buffer(gizmo_active_info, (s32) window->mousex, (s32) window->mousey);
    
    shader_active(0);
    shader_set_framebuffer(0);
  }
  
  gizmo_data* gizmo_data = &game_editor->gizmo_data;
  b32 gizmo_active 	 = (gizmo_data->state & GIZMO_ACTIVE);
  
  if(gizmo_active) {
    if(window->mouse_button_left_state & MOUSE_BUTTON_LEFT_UP) {
      gizmo_data->state = 0;
    }
  } else {
    if(gizmo_active_info[0] || gizmo_active_info[1] || gizmo_active_info[2]) {
      vec3 plane_normal;
      gizmo_data->state = (gizmo_active) ? (GIZMO_HOT | GIZMO_ACTIVE) : GIZMO_HOT;
      
      if(gizmo_active_info[0] != 0 && gizmo_active_info[1] != 0) {
	gizmo_data->state |= GIZMO_ALL_AXIS;
	plane_normal = vec3_normalize(get_camera_ray(proj_matrix, view_matrix));
      } else if(gizmo_active_info[0] != 0) {
	gizmo_data->state |= GIZMO_X_AXIS;
	plane_normal = {0.0f, 1.0f, 0.0f};
      } else if(gizmo_active_info[1] != 0) {
	gizmo_data->state |= GIZMO_Y_AXIS;
	plane_normal = {view_matrix[2], view_matrix[6], view_matrix[10]};
      } else if(gizmo_active_info[2] != 0) {
	gizmo_data->state |= GIZMO_Z_AXIS;
	plane_normal = {0.0f, 1.0f, 0.0f};
      }
      
      b32 do_active = (window->mouse_button_left_state & MOUSE_BUTTON_LEFT_DOWN) && game_editor->ignore_cursor == false;
      if(do_active) {
	write_action(game_editor, action_type::ENTITY_SCALE, recovery_type::DO);
	gizmo_data->state 	|= GIZMO_ACTIVE;
	gizmo_data->plane_normal = plane_normal;
	
	vec3 cursor_hit_position = hit_plane(game_editor->cursor_ray, camera_position, gizmo_data->plane_normal, gizmo_position);
        gizmo_data->cursor_to_origin_distance = distance(gizmo_position, cursor_hit_position);
      }
    } else {
      gizmo_data->state &= ~GIZMO_HOT;
    }
  }
  
  if(gizmo_active) {
    vec3 cursor_hit_position = hit_plane(game_editor->cursor_ray, camera_position, gizmo_data->plane_normal, gizmo_position);
    r32  scale_factor = (distance(gizmo_position, cursor_hit_position) / gizmo_data->cursor_to_origin_distance);
    vec3 vec3_factor  = {1.0f, 1.0f, 1.0f};
    
    if(gizmo_data->state & GIZMO_ALL_AXIS) {
      move_scale = {scale_factor, scale_factor, scale_factor};
    } else if(gizmo_data->state & GIZMO_X_AXIS) {
      move_scale = {scale_factor, 1.0f, 1.0f};
    } else if(gizmo_data->state & GIZMO_Y_AXIS) {
      move_scale = {1.0f, scale_factor, 1.0f};
    } else if(gizmo_data->state & GIZMO_Z_AXIS) {
      move_scale = {1.0f, 1.0f, scale_factor};
    }
    
    gizmo_data->cursor_to_origin_distance = distance(gizmo_position, cursor_hit_position);
  }
  
  renderer_buffer_data* scene_buffers = &game_editor->scene_buffers;
  shader_set_framebuffer(scene_buffers->frame_buffer);
  shader_active(shader);  
  shader_disable(GL_DEPTH_TEST);
  
  draw_gizmo_scale_arrow(game_editor, style.x_axis_color, gizmo_position, game_editor->gizmo_xaxis_matrix, gizmo_size, (gizmo_data->state & GIZMO_ACTIVE) && (gizmo_data->state & GIZMO_X_AXIS));
  draw_gizmo_scale_arrow(game_editor, style.y_axis_color, gizmo_position, game_editor->gizmo_yaxis_matrix, gizmo_size, (gizmo_data->state & GIZMO_ACTIVE) && (gizmo_data->state & GIZMO_Y_AXIS));
  draw_gizmo_scale_arrow(game_editor, style.z_axis_color, gizmo_position, game_editor->gizmo_zaxis_matrix, gizmo_size, (gizmo_data->state & GIZMO_ACTIVE) && (gizmo_data->state & GIZMO_Z_AXIS));
  
  draw_gizmo_scale_circle(game_editor, camera, style.scale_color, gizmo_position, gizmo_size);
			 
  shader_enable(GL_DEPTH_TEST);
  if(gizmo_active) {
    shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), style.triangle_color);
    
    mat4 model_matrix;
    imat4(model_matrix);
    
    mat_scale(model_matrix, gizmo_data->delta_scale);
    mat_translate(model_matrix, gizmo_position);
    
    shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  }
  
  shader_active(0);
  shader_set_framebuffer(0);
  
  return(move_scale);
}

internal quat
gizmo_rotate_editor(game_editor* game_editor, game_window* window, vec3 gizmo_position, game_camera* camera, r32 round_value = 0.0f,
		    action_type action = action_type::ENTITY_ROTATE, u32 draw_axis_flags = GIZMO_DRAW_ALL_AXIS) {
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_draw_data* render_data = &game_editor->editor_render_data;

  quat move_rotation_quat = {0.0f, 0.0f, 0.0f, 1.0f};
  vec3 camera_position = camera->position;
  
  gizmo_animation_style animation;
  gizmo_style style;
  gizmo_style default_style;
  
  r32 gizmo_size = calculate_gizmo_size(gizmo_position, camera_position);

  {
    renderer_buffer_data* editor_buffers = &game_editor->editor_buffers;
    shader_set_framebuffer(editor_buffers->frame_buffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_active(shader);  
  
    draw_gizmo_circle(game_editor, camera, style.x_axis_color, gizmo_position, game_editor->gizmo_xaxis_matrix, gizmo_size);
    draw_gizmo_circle(game_editor, camera, style.y_axis_color, gizmo_position, game_editor->gizmo_yaxis_matrix, gizmo_size);
    draw_gizmo_circle(game_editor, camera, style.z_axis_color, gizmo_position, game_editor->gizmo_zaxis_matrix, gizmo_size);
    shader_active(0);
  }

  GLubyte gizmo_active_info[3];
  get_color_from_buffer(gizmo_active_info, (s32) window->mousex, (s32) window->mousey);
  shader_set_framebuffer(0);
  
  gizmo_data* gizmo_data = &game_editor->gizmo_data;
  b32 gizmo_active = (gizmo_data->state & GIZMO_ACTIVE);

  if(gizmo_active) {
    if(window->mouse_button_left_state & MOUSE_BUTTON_LEFT_UP) {
      gizmo_data->state = 0;
    }
  } else {
    if(gizmo_active_info[0] || gizmo_active_info[1] || gizmo_active_info[2]) {
      vec3 plane_normal;
      gizmo_data->state = GIZMO_HOT;
    
      if(gizmo_active_info[0] != 0) {
	plane_normal = {1.0f, 0.0f, 0.0f};
	gizmo_data->state |= GIZMO_X_AXIS;
      } else if(gizmo_active_info[1] != 0) {
	plane_normal = {0.0f, 1.0f, 0.0f};
	gizmo_data->state |= GIZMO_Y_AXIS;
      } else if(gizmo_active_info[2] != 0) {
	plane_normal = {0.0f, 0.0f, 1.0f};
	gizmo_data->state |= GIZMO_Z_AXIS;
      }
    
      b32 do_active = (window->mouse_button_left_state & MOUSE_BUTTON_LEFT_DOWN) && game_editor->ignore_cursor == false;
      if(do_active) {
	write_action(game_editor, action, recovery_type::DO);
	gizmo_data->state	   |= GIZMO_ACTIVE;
	gizmo_data->plane_normal    = plane_normal;
	gizmo_data->origin_position = hit_plane(game_editor->cursor_ray, camera_position, gizmo_data->plane_normal, gizmo_position);
	gizmo_data->cursor_to_origin_position = gizmo_data->origin_position;
      }
    } else {
      gizmo_data->state &= ~GIZMO_HOT;
    }
  }
  
  vec3 rotation_euler = {};
  vec3 hit_position = hit_plane(game_editor->cursor_ray, camera_position, gizmo_data->plane_normal, gizmo_position);
  if(gizmo_active) {
    if(gizmo_data->state & GIZMO_X_AXIS) {
      vec2 v2_origin_gizmo   = {gizmo_position.y, 	       		 gizmo_position.z};
      vec2 v2_origin_hit     = {gizmo_data->origin_position.y, 		 gizmo_data->origin_position.z};
      vec2 v2_curr_frame_hit = {hit_position.y, 	       		 hit_position.z};
      vec2 v2_last_frame_hit = {gizmo_data->cursor_to_origin_position.y, gizmo_data->cursor_to_origin_position.z};
      
      r32 last_frame_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_last_frame_hit));
      r32 curr_frame_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_curr_frame_hit));
      
      r32 origin_hit_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_origin_hit));
      r32 apply_rotation = (curr_frame_rotation - origin_hit_rotation) - (last_frame_rotation - origin_hit_rotation);
      
      rotation_euler.z = apply_rotation;
    } else if(gizmo_data->state & GIZMO_Y_AXIS) {
      vec2 v2_origin_gizmo   = {gizmo_position.x, 	       		 gizmo_position.z};
      vec2 v2_origin_hit     = {gizmo_data->origin_position.x, 		 gizmo_data->origin_position.z};
      vec2 v2_curr_frame_hit = {hit_position.x, 	       		 hit_position.z};
      vec2 v2_last_frame_hit = {gizmo_data->cursor_to_origin_position.x, gizmo_data->cursor_to_origin_position.z};
      
      r32 last_frame_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_last_frame_hit));
      r32 curr_frame_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_curr_frame_hit));
      
      r32 origin_hit_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_origin_hit));
      r32 apply_rotation = (curr_frame_rotation - origin_hit_rotation) - (last_frame_rotation - origin_hit_rotation);
      
      rotation_euler.y = -apply_rotation;
    } else if(gizmo_data->state & GIZMO_Z_AXIS) {
      vec2 v2_origin_gizmo   = {gizmo_position.x, 	       		 gizmo_position.y};
      vec2 v2_origin_hit     = {gizmo_data->origin_position.x, 		 gizmo_data->origin_position.y};
      vec2 v2_curr_frame_hit = {hit_position.x, 	       		 hit_position.y};
      vec2 v2_last_frame_hit = {gizmo_data->cursor_to_origin_position.x, gizmo_data->cursor_to_origin_position.y};
      
      r32 last_frame_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_last_frame_hit));
      r32 curr_frame_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_curr_frame_hit));
      
      r32 origin_hit_rotation = to_degrees(angle_vec2(v2_origin_gizmo, v2_origin_hit));
      r32 apply_rotation = (curr_frame_rotation - origin_hit_rotation) - (last_frame_rotation - origin_hit_rotation);
      
      rotation_euler.x = apply_rotation;
    }

    vec3 rotation_radians = {};
    rotation_radians.x = to_radians(rotation_euler.x);
    rotation_radians.y = to_radians(rotation_euler.y);
    rotation_radians.z = to_radians(rotation_euler.z);
    move_rotation_quat = quat_from_euler(rotation_radians);
    
    gizmo_data->cursor_to_origin_position = hit_position;
  }

  renderer_buffer_data* scene_buffers = &game_editor->scene_buffers;
  shader_set_framebuffer(scene_buffers->frame_buffer);
  shader_active(shader);
  shader_disable(GL_DEPTH_TEST);

  if(gizmo_active == false) {
    draw_gizmo_circle(game_editor, camera, style.x_axis_color, gizmo_position, game_editor->gizmo_xaxis_matrix, gizmo_size);
    draw_gizmo_circle(game_editor, camera, style.y_axis_color, gizmo_position, game_editor->gizmo_yaxis_matrix, gizmo_size);
    draw_gizmo_circle(game_editor, camera, style.z_axis_color, gizmo_position, game_editor->gizmo_zaxis_matrix, gizmo_size);
  } else {
    // @incomplete
  }
  
  shader_enable(GL_DEPTH_TEST);
  shader_active(0);
  shader_set_framebuffer(0);
  
  return(move_rotation_quat);
}

//
// displacement:
//

internal void
game_editor_add_displacement(game_editor* game_editor, game_window* window) {
  displacement_editor* editor = &game_editor->displacement_editor;
  /*
  game_level* active_level = game_editor->active_level;
  for(u32 i = 0; i < editor->select_count; i++) {
    value_index it = editor->select_array[i];
    r32 x = static_cast<r32>(it.data[0]) * DISPLACEMENT_TOTAL_SIZE;
    r32 z = static_cast<r32>(it.data[1]) * DISPLACEMENT_TOTAL_SIZE;
    
    vec3 displacement_position = {x, 0.0f, z};
    displacement* displacement = instantiate_displacement(active_level, it.value);
    displacement->index    = it;
    displacement->position = displacement_position;
    create_mesh_displacement(displacement, mesh_catalog);
    
    // @temporary:
    displacement_mesh* mesh_data = &displacement->mesh_data;
    catalog_result result = get_texture_by_path(texture_catalog, string_arena, DEFAULT_DISPLACEMENT_TEXTURE_PATH);
    mesh_data->texture_array[0] = result.texture; // @temporary
    mesh_data->texture_count    = 1;
    
    renderer_submit_displacement_mesh(renderer, mesh_catalog, mesh_data);
  }
  */
  editor->select_count = 0;
}

#if 0
internal void
gizmo_height_translate_editor(game_editor* game_editor, game_window* window, game_camera* camera, mat4 view_matrix) {
  displacement_editor* editor = &game_editor->displacement_editor;
  vec3 camera_position = camera->position;
    
  gizmo_animation_style animation;
  gizmo_style default_style;
  gizmo_style style;

  gizmo_data* gizmo_data   = &game_editor->gizmo_data;
  b32 	      gizmo_active = (gizmo_data->state & GIZMO_ACTIVE);
  r32 gizmo_size = 0.25f;
  
  vec3 active_index_position = {};
  
  if(gizmo_active) {
    if(window->mouse_button_left_state & MOUSE_BUTTON_LEFT_UP) {
      editor->active_sample = NULL;
      gizmo_data->state     = 0;
    }
  } else {
    temporary_pool* sample_bucket = editor->sample_pool;
    sample_data* active_sample = 0;
    while(sample_bucket) {
      sample_data* sample_array = reinterpret_cast<sample_data*>(sample_bucket->buffer);
      u32 max_sample_count = (sample_bucket->buffer_size / sizeof(sample_data));
      
      for(u32 i = 0; i < max_sample_count; i++) {
	sample_data* it = &sample_array[i];
	vec3 sample_position = it->position;
	  
	if(hit_sphere(sample_position, gizmo_size, camera_position, game_editor->cursor_ray)) {
	  active_sample = it;
	  active_index_position = sample_position;
	  break;
	}
      }
      
      sample_bucket = sample_bucket->next;
    }
    
    if(active_sample) { 
      gizmo_data->state = (gizmo_active) ? (GIZMO_HOT | GIZMO_ACTIVE) : GIZMO_HOT;

      if(window->mouse_button_left_state & MOUSE_BUTTON_LEFT_DOWN) {
	editor->active_sample = active_sample;
	vec3 sample_position = active_sample->position;
	vec3 plane_normal = {view_matrix[2], view_matrix[6], view_matrix[10]};
	
	gizmo_data->origin_position = hit_plane(game_editor->cursor_ray, camera_position, plane_normal, sample_position);
	gizmo_data->cursor_to_origin_position = (sample_position - gizmo_data->origin_position);
	gizmo_data->delta_position = gizmo_data->origin_position;
	
	gizmo_data->state    |= GIZMO_ACTIVE;
      }
    } else {
      gizmo_data->state &= ~GIZMO_HOT;
    }
  }
  
  if(editor->active_sample) { // @incomplete:
    vec3 plane_normal = {view_matrix[2], view_matrix[6], view_matrix[10]};
    vec3 sample_position = editor->active_sample->position;
    
    vec3 cursor_hit_point = hit_plane(game_editor->cursor_ray, camera_position, plane_normal, sample_position);
    vec3 move_position = cursor_hit_point - gizmo_data->delta_position;
    
    move_position = {0.0f, move_position.y, 0.0f};
    editor->active_sample->position.y += move_position.y;
    
    gizmo_data->delta_position = cursor_hit_point;
  }
  
  {
    shader_set_framebuffer(renderer->scene_frame_buffer, false);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_active(&renderer->gizmo_shader);
    
    temporary_pool* sample_bucket = editor->sample_pool;
    while(sample_bucket) {
      sample_data* sample_array = reinterpret_cast<sample_data*>(sample_bucket->buffer);
      u32 max_sample_count	= (sample_bucket->buffer_size / sizeof(sample_data));
      
      for(u32 i = 0; i < max_sample_count; i++) {
	sample_data* it = &sample_array[i];
	vec3 sample_position = it->position;

	if(editor->active_sample && it == editor->active_sample) {
	  vec4 gizmo_color = {1.0f, 0.0f, 0.0f, 1.0f};
	  draw_gizmo_sphere(game_editor, renderer, gizmo_color, sample_position, gizmo_size);

	  vec3 min = { 0.0f, -100.0f, 0.0f };
	  vec3 max = { 0.0f,  100.0f, 0.0f };
	  renderer_draw_line(renderer, min, max);
  
	} else {
	  vec4 gizmo_color = style.y_axis_color;
	    
	  shader_disable(GL_DEPTH_TEST);
	  draw_gizmo_sphere(game_editor, renderer, gizmo_color, sample_position, gizmo_size);
	  shader_enable(GL_DEPTH_TEST);
	}
      }
      
      sample_bucket = sample_bucket->next;
    }
    
    shader_active(0);
    shader_set_framebuffer(0);
  }
}

inline internal void
adjust_displacement_vertex_at(displacement_mesh* current_mesh, displacement_mesh* target_mesh, u32 current_index, u32 target_index) {
  r32 current_height = current_mesh->position_array[current_index + 1];
  r32 target_height  = target_mesh->position_array[target_index + 1];
  
  // @incomplete:
  r32 max_height = current_height;
  r32 min_height = target_height;
  if(max_height < target_height) {
    max_height = target_height;
    min_height = current_height;
  }
  
  r32 distance = (max_height - min_height);
  r32 final_height = min_height + distance * 0.5f;
  
  current_mesh->position_array[current_index + 1] = final_height;
  target_mesh->position_array[target_index + 1]   = final_height;
}

internal void
displacement_adjust_height(game_level* active_level, displacement* current_displacement) {
  // @incomplete:
  {
    value_index target_index = current_displacement->index;
    target_index.data[1] += 1;

    displacement* target_displacement = get_displacement_by_id(active_level, target_index.value);
    if(target_displacement) {
      u32 current_z = DISPLACEMENT_ROW_COUNT - 1;
      u32 target_z  = 0;
    
      displacement_mesh* current_mesh = &current_displacement->mesh_data;
      displacement_mesh* target_mesh  = &target_displacement->mesh_data;
      for(u32 x = 0; x < DISPLACEMENT_ROW_COUNT; x++) {
	u32 current_index = (x + current_z * DISPLACEMENT_ROW_COUNT) * 3;
	u32 target_index  = (x + target_z  * DISPLACEMENT_ROW_COUNT) * 3;
	
	adjust_displacement_vertex_at(current_mesh, target_mesh, current_index, target_index);
      }

      renderer_update_displacement(current_mesh);
      renderer_update_displacement(target_mesh);
    }
  }
  {
    
    value_index target_index = current_displacement->index;
    target_index.data[0] += 1;

    displacement* target_displacement = get_displacement_by_id(active_level, target_index.value);
    if(target_displacement) {
      u32 current_x = DISPLACEMENT_ROW_COUNT - 1;
      u32 target_x  = 0;
    
      displacement_mesh* current_mesh = &current_displacement->mesh_data;
      displacement_mesh* target_mesh  = &target_displacement->mesh_data;
      for(u32 z = 0; z < DISPLACEMENT_ROW_COUNT; z++) {
	u32 current_index = (current_x + z * DISPLACEMENT_ROW_COUNT) * 3;
	u32 target_index  = (target_x  + z * DISPLACEMENT_ROW_COUNT) * 3;

	adjust_displacement_vertex_at(current_mesh, target_mesh, current_index, target_index);
      }

      renderer_update_displacement(current_mesh);
      renderer_update_displacement(target_mesh);
    }
  }
}

internal void
displacement_draw_selection(game_editor* game_editor, renderer* renderer, game_level* active_level) {
  displacement_editor* editor = &game_editor->displacement_editor;

  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_draw_data* render_data = &game_editor->editor_render_data;
  shader_program* shader = game_editor->editor_shader;
  
  vec4 line_color = {0.0f, 1.0f, 0.0f, 1.0f};
  mat4 model_matrix;

  renderer_buffer_data* scene_buffers = &game_editor->scene_buffers;
  shader_set_framebuffer(scene_buffers->frame_buffer);
  shader_active(shader);
  
  imat4(model_matrix);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), line_color); 
  
  for(u32 i = 0; i < editor->select_count; i++) {
    value_index it = editor->select_array[i];
    displacement* displacement = get_displacement_by_id(active_level, it.value);
    
    renderer_draw_displacement_mesh(renderer, &displacement->mesh_data, true);
  }
  
  shader_active(0);
  shader_set_framebuffer(0);
}

internal void
displacement_load_sample_data(game_editor* game_editor, displacement* displacement) {
  displacement_editor* editor = &game_editor->displacement_editor;
  game_level* active_level = game_editor->active_level;
  
  displacement_mesh* mesh_data = &displacement->mesh_data;
  const u32 INDEX_ADD = DISPLACEMENT_ROW_COUNT / SAMPLE_ROW_COUNT;
  for(u32 z = 0; z < DISPLACEMENT_ROW_COUNT; z += INDEX_ADD) {
    for(u32 x = 0; x < DISPLACEMENT_ROW_COUNT; x += INDEX_ADD) {
      u32 vertex_index = (x + z * DISPLACEMENT_ROW_COUNT);
      
      vec3 sample_position;
      memcpy(&sample_position, &mesh_data->position_array[vertex_index * 3], sizeof(vec3));
      
      value_index vertex_terrain_index;
      r32 position_x = static_cast<r32>(displacement->index.data[0]) * DISPLACEMENT_TOTAL_SIZE;
      r32 position_z = static_cast<r32>(displacement->index.data[1]) * DISPLACEMENT_TOTAL_SIZE;
      
      vertex_terrain_index.data[0] = static_cast<u16>(position_x + static_cast<r32>(x) * DISPLACEMENT_VERTEX_SIZE);
      vertex_terrain_index.data[1] = static_cast<u16>(position_z + static_cast<r32>(z) * DISPLACEMENT_VERTEX_SIZE);
      
      sample_data new_sample;
      new_sample.index    = vertex_terrain_index.value;
      new_sample.position = sample_position;
      add_sample_data(editor->sample_pool, new_sample);
    }
  } 
}

internal void
displacement_select_sample_handler(game_editor* game_editor, game_window* window, game_level* active_level) {
  displacement_editor* editor = &game_editor->displacement_editor;
  game_camera* camera = &active_level->camera;
  vec3 ray_origin = camera->position;
  vec3 ray_vector = game_editor->cursor_ray;
  
  displacement* active_displacement = 0;
  size_t displacement_count = hmlen(active_level->displacement_array);
  for(u32 i = 0; i < displacement_count; i++) {
    displacement* displacement = &active_level->displacement_array[i].value;
    
    displacement_mesh* mesh_data = &displacement->mesh_data;
    for(u32 z = 0; z < DISPLACEMENT_ROW_COUNT-1; z++) {
      for(u32 x = 0; x < DISPLACEMENT_ROW_COUNT-1; x++) {
	u32 v0 = (x + z * DISPLACEMENT_ROW_COUNT);
        u32 v1 = ((x + 1) + z * DISPLACEMENT_ROW_COUNT);
        u32 v2 = (x + (z + 1) * DISPLACEMENT_ROW_COUNT);
        u32 v3 = ((x + 1) + (z + 1) * DISPLACEMENT_ROW_COUNT);
	
	vec3 ip0, ip1, ip2;
	vec3 jp0, jp1, jp2;
	
	memcpy(&ip0, &mesh_data->position_array[v0 * 3], sizeof(vec3));
	memcpy(&ip1, &mesh_data->position_array[v1 * 3], sizeof(vec3));
	memcpy(&ip2, &mesh_data->position_array[v2 * 3], sizeof(vec3));
	memcpy(&jp0, &mesh_data->position_array[v2 * 3], sizeof(vec3));
	memcpy(&jp1, &mesh_data->position_array[v1 * 3], sizeof(vec3));
	memcpy(&jp2, &mesh_data->position_array[v3 * 3], sizeof(vec3));
	
	vec3 hit_position;
	if(hit_triangle(ray_origin, ray_vector, ip0, ip1, ip2, &hit_position) || hit_triangle(ray_origin, ray_vector, jp0, jp1, jp2, &hit_position)) {
	  active_displacement = displacement;
	  editor->select_array[editor->select_count] = active_displacement->index;
	  editor->select_count += 1;
	  break;
	}
      }
    }
    
    if(active_displacement) {
      // printf("select id: %u\n", active_displacement->index.value);
      displacement_load_sample_data(game_editor, active_displacement);
      break;
    }
  }  
}

internal void
displacement_vertex_texture_editor(game_editor* game_editor, game_window* window, game_level* active_level) {
  displacement_editor* editor = &game_editor->displacement_editor;
  
  if(game_editor->ignore_cursor == false && window->mouse_button_left_state & MOUSE_BUTTON_LEFT_DOWN) {
     editor->select_count = 0;
     displacement_select_sample_handler(game_editor, window, active_level);
  }
}

internal void
displacement_generate_sample_array_by_mesh(game_editor* game_editor, game_level* active_level) {
  displacement_editor* editor   = &game_editor->displacement_editor;
  temporary_pool* sample_bucket = editor->sample_pool;
  game_camera* camera = &active_level->camera;
  
  const u32 INDEX_ADD = DISPLACEMENT_ROW_COUNT / SAMPLE_ROW_COUNT;
  for(u32 i = 0; i < active_level->static_object_count; i++) {
    static_object* game_object = &active_level->static_object_array[i];
    
    entity* entity_data = get_entity_by_id(active_level, game_object->entity_id);    
    mesh* mesh_data     = static_cast<mesh*>(entity_data->mesh);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh_data->vbo);
    vertex* buffer = (vertex*) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    for(u32 i = 0; i < mesh_data->vertex_count; i+=3) {
      vec3 v0, v1, v2;
      
      memcpy(&v0, &buffer->position, sizeof(vec3)); buffer++;
      memcpy(&v1, &buffer->position, sizeof(vec3)); buffer++;
      memcpy(&v2, &buffer->position, sizeof(vec3)); buffer++;
      
      v0 = entity_data->model_matrix * v0;
      v1 = entity_data->model_matrix * v1;
      v2 = entity_data->model_matrix * v2;
      
      size_t displacement_count = hmlen(active_level->displacement_array);
      for(u32 j = 0; j < displacement_count; j++) {
	displacement* displacement = &active_level->displacement_array[j].value;
	displacement_mesh* displacement_data = &displacement->mesh_data;
	
	for(u32 z = 0; z < DISPLACEMENT_ROW_COUNT; z += INDEX_ADD) {
	  for(u32 x = 0; x < DISPLACEMENT_ROW_COUNT; x += INDEX_ADD) {
	    u32 vertex_index = (x + z * DISPLACEMENT_ROW_COUNT);
	    
	    vec3 sample_position;
	    memcpy(&sample_position, &displacement_data->position_array[vertex_index * 3], sizeof(vec3));
	    
	    // @temporary:
	    vec3 hit_position;
	    vec3 ray_vector = {0.0f, -1.0f, 0.0f};
	    vec3 ray_origin = {sample_position.x, sample_position.y + 25.0f, sample_position.z};
	    if(hit_triangle(ray_origin, ray_vector, v0, v1, v2, &hit_position)) {
	      r32 height = hit_position.y;
	      
	      r32 position_x = static_cast<r32>(displacement->index.data[0]) * DISPLACEMENT_TOTAL_SIZE;
	      r32 position_z = static_cast<r32>(displacement->index.data[1]) * DISPLACEMENT_TOTAL_SIZE;
	      value_index value_index = get_index_at(position_x, position_z, x, z);
	      
	      sample_data new_sample;
	      new_sample.index    = value_index.value;
	      new_sample.position = {sample_position.x, height, sample_position.z};
	      add_sample_data(editor->sample_pool, new_sample);
	    }
	  }
	  
	}
      }
    }
    
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

internal void
displacement_vertex_kriging_editor(game_editor* game_editor, game_level* active_level) {
  displacement_editor* editor = &game_editor->displacement_editor;
  
  size_t displacement_count = hmlen(active_level->displacement_array);
  for(u32 i = 0; i < displacement_count; i++) {
    displacement* displacement   = &active_level->displacement_array[i].value;
    displacement_mesh* mesh_data = &displacement->mesh_data;
    value_index displacement_index = displacement->index;
    
    const u32 INDEX_ADD 	  = DISPLACEMENT_ROW_COUNT / SAMPLE_ROW_COUNT;
    const u32 SAMPLES_ROW_COUNT   = (DISPLACEMENT_ROW_COUNT / INDEX_ADD) + 1;
    const u32 SAMPLES_TOTAL_COUNT = SAMPLES_ROW_COUNT * SAMPLES_ROW_COUNT;
    
    kriging_data data = {};
    data.sample_array = static_cast<vec3*>(temporary_alloc(data.buffer_pool, sizeof(vec3) * SAMPLES_TOTAL_COUNT));
    memset(data.sample_array, 0, sizeof(vec3) * SAMPLES_TOTAL_COUNT);
    
    for(u32 z = 0; z < DISPLACEMENT_ROW_COUNT; z += INDEX_ADD) {
      for(u32 x = 0; x < DISPLACEMENT_ROW_COUNT; x += INDEX_ADD) {
        r32 position_x = static_cast<r32>(displacement_index.data[0]) * DISPLACEMENT_TOTAL_SIZE;
	r32 position_z = static_cast<r32>(displacement_index.data[1]) * DISPLACEMENT_TOTAL_SIZE;
	
	value_index vertex_terrain_index = get_index_at(position_x, position_z, x, z);
	sample_data* it = get_sample_data(editor->sample_pool, vertex_terrain_index.value);
	vec3 sample_position = {};
	
	if(it) {
	  sample_position = it->position;
	} else {
	  u32 vertex_index = (x + z * DISPLACEMENT_ROW_COUNT);
	  memcpy(&sample_position, &mesh_data->position_array[vertex_index * 3], sizeof(vec3));
	}
	
	data.sample_array[data.sample_count] = sample_position;
	data.sample_count += 1;
      }
    }
    
    u32 row_count   = data.sample_count + 1;
    u32 total_count = row_count * row_count;
    
    u32 matrix_values_count = (SAMPLES_TOTAL_COUNT + 1) * (SAMPLES_TOTAL_COUNT + 1);
    size_t matrix_size = matrix_values_count * sizeof(r32); 
    
    data.row_count = row_count;
    data.inverse_matrix = static_cast<r32*>(temporary_alloc(data.buffer_pool, matrix_size));
    {
      r32* sample_matrix = static_cast<r32*>(temporary_alloc(data.buffer_pool, matrix_size));
      for(u32 i = 0; i < total_count; i++) {
	sample_matrix[i] = 1.0f;
      }
      sample_matrix[total_count-1] = 0.0f;
      
      for(u32 j = 0; j < row_count - 1; j++) {
	vec3 target_sample    = data.sample_array[j];
	vec2 v2_target_sample = {target_sample.x, target_sample.z};
	for(u32 i = 0; i < row_count - 1; i++) {
	  vec3 curr_sample    = data.sample_array[i];
	  vec2 v2_curr_sample = {curr_sample.x, curr_sample.z}; 
	  
	  r32 h = distance(v2_curr_sample, v2_target_sample);
	  sample_matrix[i + j * row_count] = calculate_spherical_cov(h, data.nugget, data.sill, data.range);
	}
      }
      
      mat_inverse(data.inverse_matrix, sample_matrix, row_count, data.buffer_pool);
      
      displacement_mesh* mesh = &displacement->mesh_data;
      renderer_clear_displacement_mesh(renderer, mesh);
      displacement->mesh_data = {};
       
      catalog_result result = get_texture_by_path(texture_catalog, string_arena, DEFAULT_DISPLACEMENT_TEXTURE_PATH);
      mesh->texture_array[0] = result.texture; // @temporary
      mesh->texture_count = 1;
      
      clear_temporary_pool(mesh->buffer_pool);
      create_mesh_displacement(displacement, mesh_catalog, &data);
      renderer_submit_displacement_mesh(renderer, mesh_catalog, mesh);
    }
    
    clear_temporary_pool(data.buffer_pool);
  }
  
  for(u32 i = 0; i < displacement_count; i++) {
    displacement* displacement = &active_level->displacement_array[i].value;
    displacement_adjust_height(active_level, displacement);
  }
}

#endif

internal void
draw_displacement_grid(game_editor* game_editor, game_camera* camera) {
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_draw_data* render_data = &game_editor->editor_render_data;
  shader_program* shader = game_editor->editor_shader;
  
  const r32 MAX_FAR   = 150.0f;
  const u32 row_count = (u32) (MAX_FAR / DISPLACEMENT_TOTAL_SIZE);
  
  vec4 line_color = {0.25f, 0.25f, 0.25f, 1.0f};
  mat4 model_matrix;
  
  renderer_buffer_data* scene_buffers = &game_editor->scene_buffers;
  shader_set_framebuffer(scene_buffers->frame_buffer);
  shader_active(shader);
  
  imat4(model_matrix);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), line_color); 
  
  for(u32 i = 0; i < row_count * 2; i++) {
    r32 offset = (static_cast<r32>(i) * DISPLACEMENT_TOTAL_SIZE) - (DISPLACEMENT_TOTAL_SIZE * row_count);
    
    vec3 x0 = {offset, 0.05f, -MAX_FAR};
    vec3 x1 = {offset, 0.05f,  MAX_FAR};
    renderer_draw_line(render_data, x0, x1);
    
    vec3 z0 = {-MAX_FAR, 0.05f, offset};
    vec3 z1 = { MAX_FAR, 0.05f, offset};
    renderer_draw_line(render_data, z0, z1);
  }
    
  shader_active(0);
  shader_set_framebuffer(0);
}

internal void
displacement_select_handler(displacement_editor* editor, game_level* active_level, s16 index_x, s16 index_z, s16 select_x, s16 select_z, b32 delete_selection) {
  s16 min_x = (index_x < select_x) ? index_x : select_x;
  s16 min_z = (index_z < select_z) ? index_z : select_z;
  s16 max_x = (index_x > select_x) ? index_x : select_x;
  s16 max_z = (index_z > select_z) ? index_z : select_z;
  
  u32 x_count = (max_x - min_x) + 1;
  u32 z_count = (max_z - min_z) + 1;
  u32 total_count = x_count * z_count;
  
  if(delete_selection) {
    for(u32 x = 0; x < x_count; x++) {
      for(u32 z = 0; z < z_count; z++) {
	value_index curr_index;
	curr_index.data[0] = (min_x + x);
	curr_index.data[1] = (min_z + z);
	
	s32 delete_index = -1;
	
	for(u32 i = 0; i < editor->select_count; i++) {
	  value_index it = editor->select_array[i]; 
	  if(curr_index == it) {
	    delete_index = i;
	    break;
	  }
	}
	
	if(delete_index >= 0) {
	  u32 new_count = editor->select_count - 1;
	  if(new_count != delete_index && new_count > 0) {
	    memcpy(&editor->select_array[delete_index], &editor->select_array[delete_index + 1], MAX_DISPLACEMENT_SELECT - (delete_index + 1));
	  }

	  editor->select_count = new_count;
	}
      }
    }
  } else {    
    u32 count = 0;
    u32 index = editor->select_count;
    for(u32 x = 0; x < x_count; x++) {
      for(u32 z = 0; z < z_count; z++) {
	value_index curr_index;
	curr_index.data[0] = (min_x + x);
	curr_index.data[1] = (min_z + z);
	
	b32 ignore = (hmgeti(active_level->displacement_array, curr_index.value) >= 0);
	
	if(!ignore) {
	  for(u32 i = 0; i < editor->select_count; i++) {
	    value_index it = editor->select_array[i];
	  
	    if(curr_index == it) {
	      ignore = true;
	      break;
	    }
	  }
	}
	
	if(!ignore) {
	  editor->select_array[index] = curr_index;
	  index += 1;
	  count += 1;
	}
      }
    }
    editor->select_count += count;
  }
}

internal void
displacement_instance_handler(game_editor* game_editor, game_window* window, game_camera* camera, mat4 view_matrix) {
  displacement_editor* editor = &game_editor->displacement_editor;
  game_level* active_level = game_editor->active_level;
  gizmo_data* gizmo_data = &game_editor->gizmo_data;

  displacement_style style = {};
  vec4 gizmo_color;
    
  vec3 camera_position = camera->position;
  const r32 displacement_size = (DISPLACEMENT_ROW_COUNT-1) * DISPLACEMENT_VERTEX_SIZE;

  b32 ignore_frame = (game_editor->cursor_ray.y > 0.0f) || game_editor->ignore_cursor;
  b32 delete_selection = (window->key_shift_state & KEY_SHIFT_PRESS);
  b32 show_transparent = true;
  
  vec3 plane_position = {0.0f, 0.0f, 0.0f};
  vec3 plane_normal   = {0.0f, 1.0f, 0.0f};
  vec3 hit = hit_plane(game_editor->cursor_ray, camera_position, plane_normal, plane_position);
  
  r32 x = floor_by(hit.x, displacement_size);
  r32 z = floor_by(hit.z, displacement_size);
  s16 index_x = static_cast<s16>(x / displacement_size);
  s16 index_z = static_cast<s16>(z / displacement_size);
  
  // @temporary:
  // printf("cursor index: %i,%i\n", index_x, index_z);
  
  if(!ignore_frame && window->mouse_button_left_state & MOUSE_BUTTON_LEFT_DOWN) {
    gizmo_data->state = GIZMO_ACTIVE;
    
    value_index index = {};
    index.data[0] = index_x;
    index.data[1] = index_z;
    
    editor->selection_index = index;
  } else if(!ignore_frame && window->mouse_button_left_state & MOUSE_BUTTON_LEFT_UP) {
    s16 select_x = editor->selection_index.data[0];
    s16 select_z = editor->selection_index.data[1];
        
    displacement_select_handler(editor, active_level, index_x, index_z, select_x, select_z, delete_selection);
    gizmo_data->state = 0;
  }
  
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_draw_data* render_data = &game_editor->editor_render_data;
  renderer_buffer_data* scene_buffers = &game_editor->scene_buffers;
  
  shader_set_framebuffer(scene_buffers->frame_buffer);
  shader_active(shader);
  shader_disable(GL_DEPTH_TEST);
  
  mat4 model_matrix;
  
  imat4(model_matrix);
  shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), model_matrix);
  
  // draw selection:
  gizmo_color = style.selection_color;
  
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), gizmo_color);
  for(u32 i = 0; i < editor->select_count; i++) {
    value_index it = editor->select_array[i];
    r32 xi = static_cast<r32>(it.data[0]);
    r32 zi = static_cast<r32>(it.data[1]);
    
    vec3 min = {xi * displacement_size, 0.05f, zi * displacement_size};
    vec3 max = {min.x + displacement_size, 0.05f, min.z + displacement_size};
    
    renderer_draw_quad(render_data, min, max);
  }
  
  shader_enable(GL_DEPTH_TEST);
  gizmo_color = style.instanced_color;
  set_alpha_value(gizmo_color, 0.75f);
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), gizmo_color);
  
  size_t displacement_count = hmlen(active_level->displacement_array);
  for(u32 i = 0; i < displacement_count; i++) {
    displacement* displacement = &active_level->displacement_array[i].value;
    value_index it = displacement->index;
    r32 xi = static_cast<r32>(it.data[0]);
    r32 zi = static_cast<r32>(it.data[1]);

    vec3 min = {xi * displacement_size, 0.05f, zi * displacement_size};
    vec3 max = {min.x + displacement_size, 0.05f, min.z + displacement_size};
      
    renderer_draw_quad(render_data, min, max);
  }
  
  shader_disable(GL_DEPTH_TEST);
  // draw cursor:
  vec3 min = {x, 0.05f, z};
  vec3 max = {x + displacement_size, 0.05f, z + displacement_size};
  
  gizmo_color = style.cursor_color;
  if(delete_selection) gizmo_color = style.delete_color;
  set_alpha_value(gizmo_color, 0.75f); // if(show_transparent) gizmo_color.w = 0.75f;
  
  shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), gizmo_color);
  renderer_draw_quad(render_data, min, max);
  
  // draw over:
  if(!ignore_frame && window->mouse_button_left_state & MOUSE_BUTTON_LEFT_PRESS) {
    s16 select_x = editor->selection_index.data[0];
    s16 select_z = editor->selection_index.data[1];
    
    s16 min_x = (index_x < select_x) ? index_x : select_x;
    s16 min_z = (index_z < select_z) ? index_z : select_z;
    s16 max_x = (index_x > select_x) ? index_x : select_x;
    s16 max_z = (index_z > select_z) ? index_z : select_z;
    
    vec3 selection_min = {static_cast<r32>(min_x) * displacement_size, 0.0f, static_cast<r32>(min_z) * displacement_size};
    vec3 selection_max = {static_cast<r32>(max_x) * displacement_size + displacement_size, 0.0f, static_cast<r32>(max_z) * displacement_size + displacement_size};
    
    gizmo_color = {0.15f, 0.15f, 0.20f, 1.0f};
    if(delete_selection) gizmo_color = {0.1f, 0.05f, 0.05f, 1.0f};
    if(show_transparent) gizmo_color.w = 0.75f;
    
    shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), gizmo_color);
    renderer_draw_quad(render_data, selection_min, selection_max);
  }  
  
  shader_enable(GL_DEPTH_TEST);
  shader_active(0);
  shader_set_framebuffer(0);
}

// ENTITY EDITOR

internal void
game_editor_add_ocean(game_editor* game_editor, game_state* state, game_window* window/*, game_camera* camera, vec3 cursor_ray, mat4 view_matrix*/) {
  game_level* active_level = game_editor->active_level;
  ocean* ocean_data = &active_level->ocean_data;
  
  mesh_data ocean_mesh_data;
  create_mesh_ocean(&ocean_mesh_data, ocean_data->water_height, ocean_data->water_size, ocean_data->tile_size);
  renderer_submit_mesh(&ocean_mesh_data, &active_level->ocean_mesh);
  // clear_mesh_data(&ocean_mesh_data); @incomplete:
  
  // catalog_result result = get_texture_by_path(texture_catalog, string_arena, DEFAULT_OCEAN_TEXTURE_PATH);
  texture* ocean_dudv_texture = 0;

  ocean_data->active = true;
  ocean_data->dudv_texture = ocean_dudv_texture;
}

internal void
game_editor_add_game_object(game_editor* game_editor, game_window* window, entity_type type/*, game_camera* camera, vec3 cursor_ray, mat4 view_matrix*/) {
  game_level* active_level = game_editor->active_level;
  vec3 entity_position = {};
  
  write_action(game_editor, action_type::ENTITY_INSTANTIATE, recovery_type::DO);
  entity* new_entity = instantiate_entity(active_level, type);
  mesh_file_data file_data = {};
  
  switch(new_entity->type) {
  case entity_type::STATIC_OBJECT: {
    get_mesh_by_name(game_editor->catalog_handler, &file_data, const_string("gizmo_cube"));
  } break;
  case entity_type::SQUARE_INFO_OBJECT: {
    get_mesh_by_name(game_editor->catalog_handler, &file_data, const_string("gizmo_sphere"));
  } break;
  case entity_type::DYNAMIC_OBJECT: {
    get_mesh_by_name(game_editor->catalog_handler, &file_data, const_string("gizmo_animation"));
  } break;
  case entity_type::PLAYER_OBJECT: {
    get_mesh_by_name(game_editor->catalog_handler, &file_data, const_string("gizmo_player"));
  } break;
  }

  mesh_data mesh_data = file_data.mesh_data_array[0];
  renderer_submit_mesh(&mesh_data, &new_entity->mesh_render);
  
  string temp_string = static_to_string(new_entity->target_mesh_name);
  string_append(temp_string, static_to_string(file_data.file_path));
  update_static_string(new_entity->target_mesh_name, temp_string);
  
  // clear_mesh_file(&file_data); @incomplete
  
  update_model_matrix(new_entity);
}

internal void
delete_selected_entities(game_editor* game_editor) {
  game_level* active_level = game_editor->active_level;
  entity_editor* editor    = &game_editor->entity_editor;
  
  write_action(game_editor, action_type::ENTITY_REMOVE, recovery_type::DO);
  for(u32 i = 0; i < editor->entity_selected_count; i++) {
    entity* entity_to_delete = get_entity_by_id(active_level, editor->entity_selected_array[i]);
    
    remove_entity(active_level, entity_to_delete);
  }
  editor->entity_selected_count = 0;
}

internal b32
game_editor_is_entity_select(game_editor* game_editor, u32 entity_id) {
  entity_editor* editor = &game_editor->entity_editor;
  b32 result = false;
  u32 selected_count = editor->entity_selected_count;
  for(u32 i = 0; i < selected_count; i++) {
    u32 it = editor->entity_selected_array[i];
    if(it == entity_id) {
      result = true;
      break;
    }
  }

  return(result);
}

internal void
select_new_entity(game_editor* game_editor, entity_editor* editor, u32 entity_id, editor_action action) {
  if(action == SELECT) {
    write_action(game_editor, action_type::ENTITY_SELECT, recovery_type::DO);
    editor->entity_selected_count = 1;
    editor->entity_selected_array[0] = entity_id;
  }
	
  if(action == MULTI_SELECT) {
    write_action(game_editor, action_type::ENTITY_SELECT, recovery_type::DO);
    u32 index = editor->entity_selected_count;
    editor->entity_selected_count += 1;
    editor->entity_selected_array[index] = entity_id;
  }
}

internal void
clone_selected_entities(game_editor* game_editor, game_level* level) {
  game_level* active_level = game_editor->active_level;
  entity_editor* editor = &game_editor->entity_editor;
  
  u32 clone_entities_array[MAX_ENTITY_COUNT];
  u32 selected_count = editor->entity_selected_count;
  for(u32 i = 0; i < selected_count; i++) {
    entity* it = get_entity_by_id(active_level, editor->entity_selected_array[i]);

    if(it->type == entity_type::PLAYER_OBJECT) {
      continue;
    }
    
    entity* copy_object = instantiate_entity(active_level, it->type);
    clone_entities_array[i] = copy_object->id;

    // @incomplete:
    // copy_object->mesh_render = get_mesh_by_name(game_editor->catalog_handler, const_string("gizmo_cube"));
    
    switch(it->type) {
    case entity_type::STATIC_OBJECT: {
      static_object* object = reinterpret_cast<static_object*>(copy_object->object_pointer);
    } break;
    case entity_type::SQUARE_INFO_OBJECT: {
      square_object* object = reinterpret_cast<square_object*>(copy_object->object_pointer);
    } break;
    case entity_type::DYNAMIC_OBJECT: {
      dynamic_object* object = reinterpret_cast<dynamic_object*>(copy_object->object_pointer);
    } break;
    }
  }
  
  editor->entity_selected_count = 0; // @temporary
  for(u32 i = 0; i < selected_count; i++) {
    select_new_entity(game_editor, editor, clone_entities_array[i], MULTI_SELECT);
  }
}

internal s32
get_entity_by_cursor(game_editor* game_editor, game_window* window, game_level* active_level, vec3 camera_position) {
  s32 result = -1;
  
  mat4 model_matrix;
  imat4(model_matrix);
  
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_buffer_data* editor_buffers = &game_editor->editor_buffers;
  shader_program* shader = game_editor->editor_shader;

  shader_set_framebuffer(editor_buffers->frame_buffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shader_active(shader);
  
  size_t entity_count = hmlen(active_level->entity_array);
  for(u8 index = 0; index < entity_count; index++) {
    entity_hash* it = &active_level->entity_array[index];
    u32 entity_id   = it->key;
    entity* entity  = &it->value;
    
    vec4 color_id = {0.0f, 0.0f, 0.0f, 1.0f};
    color_id.x = static_cast<r32>((entity->id + 1)) / 255.0f; // @incomplete
    
    shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), color_id);
    shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), entity->model_matrix);
    
    renderer_draw_entity(catalog_handler, shader, entity);
  }
  
  GLubyte pixel_color[3] = {};
  get_color_from_buffer(pixel_color, (u32) window->mousex, (u32) window->mousey);
  
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  result = static_cast<s32>(pixel_color[0] - 1);
  return(result);
}

internal void
entity_select_editor(game_editor* game_editor, game_window* window, game_level* active_level, vec3 camera_position, r32 dt) {
  entity_editor* editor = &game_editor->entity_editor;
  vec4 gizmo_color = {1.0f, 1.0f, 0.0f, 1.0f}; // @temporary
  
  if(game_editor->ignore_cursor == false) {
    editor_action action = editor_action::NO_ACTION;
    s32 entity_over_id = 0;
    
    if(window->mouse_button_left_state & MOUSE_BUTTON_LEFT_UP) {
      action = (window->key_shift_state & KEY_SHIFT_PRESS) ? MULTI_SELECT : SELECT;
      entity_over_id = get_entity_by_cursor(game_editor, window, active_level, camera_position);
    }
    
    if((game_editor->gizmo_data.last_state & GIZMO_ACTIVE) && action != NO_ACTION) {
      return; // @temporary
    }
    
    if(entity_over_id >= 0) {
      b32 already_selected = false;
      
      for(u32 i = 0; i < editor->entity_selected_count; i++) {
	if(entity_over_id == editor->entity_selected_array[i]) {
	  already_selected = true;
	  break;
	} 
      }
      
      if(already_selected == false) {
	select_new_entity(game_editor, editor, entity_over_id, action);
      }
    } 
  }
  
  shader_program* shader = game_editor->editor_shader;
  catalog_handler* catalog_handler = game_editor->catalog_handler;
  renderer_buffer_data* scene_buffers = &game_editor->scene_buffers;

  shader_set_framebuffer(scene_buffers->frame_buffer);
  shader_active(shader);
  
  for(u8 i = 0; i < editor->entity_selected_count; i++) {
    u32 index = editor->entity_selected_array[i];
    entity* entity = get_entity_by_id(active_level, index);
        
    shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("model_matrix"), entity->model_matrix);
    shader_set_color(catalog_handler->shader_catalog, shader, const_string("u_color"), gizmo_color);
    // renderer_draw_entity(entity);
  }
  
  shader_active(0);
  shader_set_framebuffer(0);
}

inline internal vec3
get_center_position(entity_editor* editor, game_level* active_level) {
  entity* first_entity = get_entity_by_id(active_level, editor->entity_selected_array[0]);
  vec3 min = first_entity->position;
  vec3 max = first_entity->position;
  
  for(u32 i = 0; i < editor->entity_selected_count; i++) {
    u32 index = editor->entity_selected_array[i];
    entity* it = get_entity_by_id(active_level, index);
      
    if(min.x > it->position.x) min.x = it->position.x;
    if(min.y > it->position.y) min.y = it->position.y;
    if(min.z > it->position.z) min.z = it->position.z;
    
    if(max.x < it->position.x) max.x = it->position.x;
    if(max.y < it->position.y) max.y = it->position.y;
    if(max.z < it->position.z) max.z = it->position.z;
  }
  
  return(center(min, max));
}

internal void
entity_transform_editor(game_editor* game_editor, game_window* window, game_camera* camera, mat4 proj_matrix, mat4 view_matrix) {
  game_level* active_level = game_editor->active_level;
  entity_editor* editor = &game_editor->entity_editor;
  r32 round = (window->key_control_state & KEY_CONTROL_PRESS) ? 0.1f : 0.0f;
  
  size_t selected_entity_count = editor->entity_selected_count;

  if(selected_entity_count) {
    vec3 gizmo_position = get_center_position(editor, active_level);
    
    vec3 delta_position = {0.0f, 0.0f, 0.0f};
    quat delta_rotation = {0.0f, 0.0f, 0.0f, 1.0f};
    vec3 delta_scale    = {0.0f, 0.0f, 0.0f};

    if(editor->gizmo_editor_type == gizmo_type::TRANSLATE) {
      delta_position = gizmo_translate_editor(game_editor, window, gizmo_position, camera, view_matrix, round);
    } else if(editor->gizmo_editor_type == gizmo_type::ROTATE) {
      delta_rotation = gizmo_rotate_editor(game_editor, window, gizmo_position, camera, round);
    } else if(editor->gizmo_editor_type == gizmo_type::SCALE) {
      delta_scale = gizmo_scale_editor(game_editor, window, camera, gizmo_position, proj_matrix, view_matrix, round);
    }
    
    for(u32 i = 0; i < selected_entity_count; i++) {
      u32 index = editor->entity_selected_array[i];
      entity* active_entity = get_entity_by_id(active_level, index);

      switch(editor->gizmo_editor_type) {
      case TRANSLATE:
	active_entity->position = active_entity->position + delta_position;
	break;
      case ROTATE:
	active_entity->rotation = mul_quat(delta_rotation, active_entity->rotation); 
	break;
      case SCALE:
	active_entity->scale = active_entity->scale * delta_scale;
	break;
      }
      
      update_model_matrix(active_entity);
    } 
  }  
}

internal void
game_editor_update(game_editor* game_editor, game_state* state, game_window* window, ui_handler* ui_handler, r32 dt) {
  game_level* active_level = game_editor->active_level;
  game_camera* camera 	   = &active_level->camera;
  gizmo_data* gizmo_data   = &game_editor->gizmo_data;

  {
    shader_program* shader = game_editor->editor_shader;
    catalog_handler* catalog_handler = game_editor->catalog_handler;
    
    shader_active(shader);
    shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("proj_matrix"), state->persp_matrix);
    shader_set_matrix(catalog_handler->shader_catalog, shader, const_string("view_matrix"), state->view_matrix);
    shader_set_value(catalog_handler->shader_catalog, shader, const_string("u_camera_position"), camera->position);
    shader_set_valuef(catalog_handler->shader_catalog, shader, const_string("u_max_render_distance"), 0);
    shader_active(0);
  }
  
  game_editor->ignore_cursor   = ui_handler->cursor_over_ui;
  game_editor->ignore_keyboard = ui_handler->focus_over_ui;
  
  // @temporary: game camera movement
  {
    static r32 sensivility = 10.0f; // @temporary
    if(window->press_btns[GLFW_MOUSE_BUTTON_RIGHT]) {
      camera->yaw   -= window->curr_mouse_dir.x * sensivility * dt;
      camera->pitch += window->curr_mouse_dir.y * sensivility * dt;
    }
  
    if(camera->pitch > 89.0f)  camera->pitch = 89.0f;
    if(camera->pitch < -89.0f) camera->pitch = -89.0f;
    
    vec3 move_position = camera->position;
    r32 velocity = 2.5f * dt;
    b32 ignore_movement = window->press_keys[GLFW_KEY_LEFT_SHIFT] || game_editor->ignore_keyboard;
    if(!ignore_movement) {
      if(window->press_keys[GLFW_KEY_W]) {
	move_position += camera->look * velocity;
      }
      if(window->press_keys[GLFW_KEY_S]) {
	move_position -= camera->look * velocity;
      }
      if(window->press_keys[GLFW_KEY_A]) {
	move_position -= camera->right * velocity;
      }
      if(window->press_keys[GLFW_KEY_D]) {
	move_position += camera->right * velocity;
      }
      
      if(window->press_keys[GLFW_KEY_X]) {
	move_position.y -= velocity;
      }
      if(window->press_keys[GLFW_KEY_Z]) {
	move_position.y += velocity;
      }
    }
  
    camera->position = move_position;
  }

  game_editor->cursor_ray = get_mouse_ray(window->iwidth, window->iheight, window->mousex, window->mousey, state->persp_matrix, state->view_matrix);
  
  switch(game_editor->tool) {
  case editor_tool::EDIT_DISPLACEMENT: {
    displacement_editor* editor = &game_editor->displacement_editor;
    
    draw_displacement_grid(game_editor, camera);
    switch(editor->tool) {
    case EDIT_INSTANCE: {
      displacement_instance_handler(game_editor, window, camera, state->view_matrix);
    } break;
    case EDIT_HEIGHT: {
      // gizmo_height_translate_editor(game_editor, window, camera, state->view_matrix);
      // displacement_draw_selection(game_editor, active_level);
    } break;
    case EDIT_TEXTURE: {
      // displacement_vertex_texture_editor(game_editor, window, active_level);
      // displacement_draw_selection(game_editor, active_level);
    } break;
    }
    
  }break;
  case editor_tool::EDIT_ENTITY:{
    entity_editor* editor = &game_editor->entity_editor;
    
    entity_select_editor(game_editor, window, &state->level, camera->position, dt);
    
    if(window->typed_keys[GLFW_KEY_1]) {
      editor->gizmo_editor_type = TRANSLATE;
    } else if(window->typed_keys[GLFW_KEY_2]) {
      editor->gizmo_editor_type = ROTATE;
    } else if(window->typed_keys[GLFW_KEY_3]) {
      editor->gizmo_editor_type = SCALE;
    }
    
    entity_transform_editor(game_editor, window, camera, state->persp_matrix, state->view_matrix);
    
    if(window->typed_keys[GLFW_KEY_F] && (window->key_control_state & KEY_CONTROL_PRESS)) {
      clone_selected_entities(game_editor, active_level);
    }

    if(!(gizmo_data->state & GIZMO_ACTIVE)) {
      if(window->typed_keys[GLFW_KEY_X] && (window->key_control_state & KEY_CONTROL_PRESS)) {
	delete_selected_entities(game_editor);
      }
    }    
  }break;
  }
  
  gizmo_data->last_state = gizmo_data->state;
}

#include "editor_panel.cpp"

internal void
game_editor_ui(game_editor* game_editor, game_state* state, game_window* window, ui_handler* ui_handler, r32 dt) {
  game_level* active_level = game_editor->active_level;
  game_camera* camera 	   = &active_level->camera;
  ui_start_frame(ui_handler, dt); 
  
  list_tool_panel(game_editor, window, state, ui_handler);  
  
  ui_end_frame(ui_handler);
}

// @cleanup: ui unused stuff
#if 0
  read_action(editor, recovery_type::UNDO);
  read_action(editor, recovery_type::REDO);
  
  save_level(game_editor, active_level, "level_name");
  load_level(game_editor, mesh_catalog, texture_catalog, renderer, active_level, "data/levels/level_name");

  if(button(ui, window, header_r, dt, "Select", 3, &theme)) {
    game_editor->displacement_editor.tool = displacement_tool::EDIT_SELECTION;
  }
  header_r.x = r.x + r.w * 1;
  if(button(ui, window, header_r, dt, "Vertex", 4, &theme)) {
    game_editor->displacement_editor.tool = displacement_tool::EDIT_VERTEX;
  }
  header_r.x = r.x + r.w * 2;
  if(button(ui, window, header_r, dt, "Blend", 5, &theme)) {
    game_editor->displacement_editor.tool = displacement_tool::EDIT_BLEND;
  }

  if(game_editor->displacement_editor.tool == displacement_tool::EDIT_SELECTION) {
    displacement_editor* editor = &game_editor->displacement_editor;

    label(ui, window, r, "Displacement Editor", &theme);

    if(button(ui, window, list_r, dt, "Clear selection", 6, &theme)) {
      memset(editor->displacement_selected_array, 0, sizeof(displacement_mesh*) * editor->displacement_selected_count);
      editor->displacement_selected_count = 0;
    }
  }
  if(game_editor->displacement_editor.tool == displacement_tool::EDIT_VERTEX) {
    displacement_editor* editor = &game_editor->displacement_editor;

    label(ui, window, r, "Displacement Editor", &theme);
    
    if(button(ui, window, list_r, dt, "New Kriging", 8, &theme)) {
      if(editor->kriging_data_count < HEIGHT_DATA_MAX) {
	editor->kriging_data_count++;
	editor->active_kriging_data = 0;
      }
    }
    
    if(editor->kriging_data_count) {  
      if(button(ui, window, list_r, dt, "Kriging Recalculate", 9, &theme)) {
	displacement_vertex_kriging_editor(game_editor, active_level);
      }

      if(button(ui, window, list_r, dt, "Add Sample", 20, &theme)) {
	displacement_sample_add(editor);
      }

      if(button(ui, window, list_r, dt, "Select Samples", 21, &theme)) {
	kriging_data* krig_data = &editor->kriging_data_array[editor->active_kriging_data];
	for(u32 i = 0; i < krig_data->sample_point_count; i++) {
	  vec3* current_sample    = &krig_data->sample_point_array[i];
	  krig_data->sample_selected_array[i] = current_sample;
	}
	krig_data->sample_selected_count = krig_data->sample_point_count;
      }
      
      if(button(ui, window, list_r, dt, "Clear Selection", 22, &theme)) {
	kriging_data* krig_data = &editor->kriging_data_array[editor->active_kriging_data];
	krig_data->sample_selected_count = 0;
      } 
    }
  }  
#endif  

