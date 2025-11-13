package engine;

import static org.lwjgl.opengl.GL11.GL_FLOAT;
import static org.lwjgl.opengl.GL11.GL_TEXTURE_2D;
import static org.lwjgl.opengl.GL11.GL_TRIANGLES;
import static org.lwjgl.opengl.GL11.GL_UNSIGNED_INT;
import static org.lwjgl.opengl.GL11.glBindTexture;
import static org.lwjgl.opengl.GL11.glDrawElements;
import static org.lwjgl.opengl.GL13.GL_TEXTURE0;
import static org.lwjgl.opengl.GL13.glActiveTexture;
import static org.lwjgl.opengl.GL15.GL_ARRAY_BUFFER;
import static org.lwjgl.opengl.GL15.GL_ELEMENT_ARRAY_BUFFER;
import static org.lwjgl.opengl.GL15.GL_STATIC_DRAW;
import static org.lwjgl.opengl.GL15.glBindBuffer;
import static org.lwjgl.opengl.GL15.glBufferData;
import static org.lwjgl.opengl.GL15.glGenBuffers;
import static org.lwjgl.opengl.GL20.glEnableVertexAttribArray;
import static org.lwjgl.opengl.GL20.glVertexAttribPointer;
import static org.lwjgl.opengl.GL30.glBindVertexArray;
import static org.lwjgl.opengl.GL30.glGenVertexArrays;

import maths.rect;
import maths.vec4;

public class RenderQuad {
	public rect rect;
	public vec4 color;
	public Texture texture_array[];
	
	public int vbo;
	public int vao;
	public int ibo;
	
	public float uv_scale = 1.0f;

	public RenderQuad(float w, float h, float uv_scale, String path) {
		rect 	= new rect(0, 0, w, h);
		color 	= new vec4(1f, 1f, 1f, 1f);
		this.uv_scale = uv_scale;
		
		texture_array = new Texture[1];
		texture_array[0] = new Texture();
		texture_array[0].load_texture_file(path);
		init();
	}
	
	public RenderQuad(float x, float y, float w, float h) {
		rect 	= new rect(x, y, w, h);
		color 	= new vec4(1f, 1f, 1f, 1f);
	}
	
	public void init() {
		float texture_index = (texture_array != null) ? 1 : 0;
		float data_array[]  = {
				rect.x		   , rect.y			, color.x, color.y, color.z, color.w, 0.0f, 0.0f, texture_index,
				rect.x + rect.w, rect.y			, color.x, color.y, color.z, color.w, 1.0f * uv_scale, 0.0f, texture_index,
				rect.x		   , rect.y + rect.h, color.x, color.y, color.z, color.w, 0.0f, 1.0f * uv_scale, texture_index,
				rect.x + rect.w, rect.y + rect.h, color.x, color.y, color.z, color.w, 1.0f * uv_scale, 1.0f * uv_scale, texture_index,
		};
		
		int indices_array[] = {
				0, 1, 2,
				2, 1, 3,
		};
		
		vao = glGenVertexArrays();
		glBindVertexArray(vao);
		
		vbo = glGenBuffers();
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data_array, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * 9, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, 4 * 9, (4*2));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, false, 4 * 9, (4*2) + (4*4));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, false, 4 * 9, (4*2) + (4*4) + (4*2));
		
		ibo = glGenBuffers();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_array, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	public void render(ShaderProgram shader) {
        glBindVertexArray(vao);
        
        if(texture_array[0].texture_id[0] > 0) {
    		shader.set_uniform("u_bitmap", 0);
    		glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_array[0].texture_id[0]);
        }

		shader.set_uniform("u_animate_uv", 0f);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        if(texture_array[0].texture_id[0] > 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        
        glBindVertexArray(0);
	}
	
}
