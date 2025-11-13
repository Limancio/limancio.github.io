package engine;

import static org.lwjgl.opengl.GL20.*;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import maths.lvec2;
import maths.mat4;
import maths.vec4;

public class ShaderProgram {
	public int shader_id;
	public Map<String, Integer> uniform_cache;

	public int vertex_id;
    public int fragment_id;
    
    public ShaderProgram() {
    	uniform_cache = new HashMap<>();
	}
    
    public void load_shader_file(String path) {
    	shader_id = glCreateProgram();

        if (shader_id == 0) {
            System.err.println("Error creating shader.");
            return;
        }
        
        File file = new File(path);
        if(file.exists() == false) {
        	System.err.println("Error loading shader file. Path: " + path);
            return;
        }

        try {
	        FileInputStream input_file = new FileInputStream(file);
	        String shader_code = new String(input_file.readAllBytes());
	        String shader_data[] = shader_code.split("#frag");
	        {
	            vertex_id = glCreateShader(GL_VERTEX_SHADER);
	            glShaderSource(vertex_id, shader_data[0]);
	            glCompileShader(vertex_id);
	
	            if (glGetShaderi(vertex_id, GL_COMPILE_STATUS) == 0) {
	            	System.err.println("Error compiling Vertex Shader code: " + glGetShaderInfoLog(vertex_id, 1024));
	                glDeleteShader(vertex_id);
	            }
	            glAttachShader(shader_id, vertex_id);
	        }
	        {
	        	fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
	            glShaderSource(fragment_id, shader_data[1]);
	            glCompileShader(fragment_id);
	
	            if (glGetShaderi(fragment_id, GL_COMPILE_STATUS) == 0) {
	            	System.err.println("Error compiling Fragment Shader code: " + glGetShaderInfoLog(fragment_id, 1024));
	                glDeleteShader(fragment_id);
	            }
	            glAttachShader(shader_id, fragment_id);
	        }

	        glLinkProgram(shader_id);
	        if (glGetProgrami(shader_id, GL_LINK_STATUS) == 0) {
	        	System.err.println("Error linking Shader code: " + glGetProgramInfoLog(shader_id, 1024));
	        }

	        glValidateProgram(shader_id);
	        if (glGetProgrami(shader_id, GL_VALIDATE_STATUS) == 0) {
	            System.err.println("Warning validating Shader code: " + glGetProgramInfoLog(shader_id, 1024));
	        }

            glDeleteShader(vertex_id);
            glDeleteShader(fragment_id);
	      
			input_file.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
    }
    
    public void cleanup() {
    	glUseProgram(0);
    	glDeleteProgram(shader_id);
    }
    
	public void bind() {
        glUseProgram(shader_id);
	}
	
	public void unbind() {
        glUseProgram(0);
	}
	
	private int get_uniform_location(String uniform_name) {
		int location = -1;
		
		if(uniform_cache.containsKey(uniform_name)) {
			location = uniform_cache.get(uniform_name);
		} else {
	        location = glGetUniformLocation(shader_id, uniform_name);
	        if (location >= 0) {
	        	uniform_cache.put(uniform_name, location);
	        }
		}
		
        return(location);
	}
	
	public void set_uniform(String uniform_name, mat4 matrix) {
        int location = get_uniform_location(uniform_name);
        if(location >= 0) {
        	glUniformMatrix4fv(location, false, matrix.data);
        }
    }
	
	public void set_uniform(String uniform_name, int value) {
        int location = get_uniform_location(uniform_name);
        if(location >= 0) {
        	glUniform1i(location, value);
        }
    }
	
	public void set_uniform(String uniform_name, lvec2 mouse_pos) {
        int location = get_uniform_location(uniform_name);
        if(location >= 0) {
        	glUniform2f(location, (float) mouse_pos.x, (float) mouse_pos.y);
        }
	}

	public void set_uniform(String uniform_name, vec4 value) {
        int location = get_uniform_location(uniform_name);
        if(location >= 0) {
        	glUniform4f(location, value.x, value.y, value.z, value.w);
        }
	}

	public void set_uniform(String uniform_name, float value) {
        int location = get_uniform_location(uniform_name);
        if(location >= 0) {
        	glUniform1f(location, value);
        }
	}
    
}
