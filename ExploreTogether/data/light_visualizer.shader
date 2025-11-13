#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

out vec2 v_uv;
out vec4 v_color;
out vec2 v_frag_position;

uniform mat4 proj_matrix;

void main() {	
     gl_Position = proj_matrix * vec4(position.x, position.y, 0.0f, 1.0);
     v_uv        = uv;
     v_color 	 = color;
     v_frag_position = vec2(position.x, position.y);
}

#frag
#version 330 core

out vec4 frag_color;

in vec2 v_uv;
in vec4 v_color;
in vec2 v_frag_position;

uniform sampler2D u_texture_font;
uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_normal;

uniform float u_draw_type;
uniform vec3  u_view_position;

uniform float u_has_diffuse;
uniform float u_has_specular;
uniform float u_has_normal;

struct point_light {
  vec3 position;
  float radius;
  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  
  float constant;
  float linear;
  float quadratic;
};

struct dir_light {
  vec3 direction;
  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform dir_light u_directional_light;
uniform point_light u_point_light_array[5];
uniform float u_point_light_count;

vec3 calculate_direction_light(dir_light light, vec3 normal, vec3 diffuse_color, vec3 specular_color, vec3 normal_map, vec3 frag_position, vec3 view_dir) {
  // diffuse
  vec3 light_dir = normalize(-light.direction);
  float diffuse_shading = max(dot(normal, light_dir), 0.0);

  // specular
  float material_shininess = 80.0;
  vec3 reflect_dir = reflect(-light_dir, normal);  
  float specular_shading = pow(max(dot(view_dir, reflect_dir), 0.0), material_shininess);
  
  vec3 ambient  = light.ambient  * diffuse_color;
  vec3 diffuse  = light.diffuse  * diffuse_shading  * diffuse_color;  
  vec3 specular = light.specular * specular_shading * specular_color;
  
  return(ambient + diffuse + specular);
}
vec3 calculate_point_light(point_light light, vec3 normal, vec3 diffuse_color, vec3 specular_color, vec3 normal_map, vec3 frag_position, vec3 view_dir) {
  // diffuse
  vec3 light_dir = normalize(light.position - frag_position);
  float diffuse_shading = max(dot(normal, light_dir), 0.0);

  // specular
  float material_shininess = 80.0;
  vec3 reflect_dir = reflect(-light_dir, normal);  
  float specular_shading = pow(max(dot(view_dir, reflect_dir), 0.0), material_shininess);
        
  // attenuation
  float distance    = length(light.position - frag_position);
  float attenuation = light.radius / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
  vec3 ambient  = light.ambient  * diffuse_color;
  vec3 diffuse  = light.diffuse  * diffuse_shading  * diffuse_color;  
  vec3 specular = light.specular * specular_shading * specular_color;
  
  ambient  *= attenuation;  
  diffuse  *= attenuation;
  specular *= attenuation;
    
  return(ambient + diffuse + specular);
}

void main() {
  int type = int(u_draw_type + 0.5);
  
  if(type == 1) {
    frag_color = vec4(texture(u_texture_font, v_uv).r); 
  } else if(type == 2) {
    frag_color = texture(u_texture_diffuse, v_uv); 
  } else if(type == 3) {
    frag_color = v_color; 
  } else if(type == 4) {
    int light_count = int(u_point_light_count + 0.5);
    
    vec3 diffuse_color  = vec3(v_color.rgb);
    float alpha = 0.0f;
    
    for(int i = 0; i < light_count; i++) {
      // attenuation
      vec2 light_position = vec2(u_point_light_array[i].position.x, u_point_light_array[i].position.y);
      float distance    = length(light_position - v_frag_position);
      float attenuation = u_point_light_array[i].radius / (u_point_light_array[i].constant + u_point_light_array[i].linear * distance + u_point_light_array[i].quadratic * (distance * distance));
      diffuse_color = diffuse_color * attenuation * u_point_light_array[i].diffuse;
      
      alpha = attenuation;
    }

    if(alpha < 0.1)
      discard;
    
    frag_color = vec4(diffuse_color, alpha);
  } else if(type == 5) {
    
    vec3 diffuse_color  = vec3(1.0, 1.0, 1.0);
    vec3 specular_color = vec3(1.0, 1.0, 1.0);
    vec3 normal_map     = vec3(1.0, 1.0, 1.0);

    if(u_has_diffuse != 0.0) {
      diffuse_color = texture(u_texture_diffuse, v_uv).rgb;
    }
    if(u_has_specular != 0.0) {
      specular_color = texture(u_texture_specular, v_uv).rgb;
    }
    if(u_has_normal != 0.0) {
      normal_map = texture(u_texture_normal, v_uv).rgb;
    }

    vec3 frag_position = vec3(v_frag_position.x, v_frag_position.y, 0.0);
    vec3 view_dir = normalize(u_view_position - frag_position);
    vec3 normal = normal_map;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 result = vec3(0.0);
    result += calculate_direction_light(u_directional_light, normal, diffuse_color, specular_color, normal_map, frag_position, view_dir);
    
    int point_light_count = int(u_point_light_count + 0.5);
    for(int i = 0; i < point_light_count; i++) {
      result += calculate_point_light(u_point_light_array[i], normal, diffuse_color, specular_color, normal_map, frag_position, view_dir);
    }
    frag_color = vec4(result, 1.0);
  }
}
