package engine;

import static org.lwjgl.opengl.GL20.*;
import static org.lwjgl.opengl.GL30.*;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

import org.lwjgl.BufferUtils;
import org.lwjgl.stb.*;

public class Texture {
	public int width;
	public int height;
	public int texture_id[];
	public String path;
	
	public Texture() {
		texture_id = new int[1];
		texture_id[0] = 0;
	}
	
	public void 
	load_texture_file(String path) {
		this.path = path;
		IntBuffer x = BufferUtils.createIntBuffer(1);
		IntBuffer y = BufferUtils.createIntBuffer(1);
		IntBuffer channels = BufferUtils.createIntBuffer(1);
		
		STBImage.stbi_set_flip_vertically_on_load(true);
		ByteBuffer image = STBImage.stbi_load(path, x, y, channels, STBImage.STBI_rgb_alpha);
		
		if(image == null) {
			System.err.println("Could not decode image file ["+ path +"]: ["+ STBImage.stbi_failure_reason() +"]");
		} else {
			width  = x.get(0);
			height = y.get(0);
			
		    glGenTextures(texture_id);
		    glBindTexture(GL_TEXTURE_2D, texture_id[0]);
		    
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		    
		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		    glGenerateMipmap(GL_TEXTURE_2D);
		    
		    glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	
}
