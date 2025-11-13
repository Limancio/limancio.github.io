package game;

import engine.RenderQuadAnim;
import engine.ShaderProgram;
import engine.Texture;
import maths.mat4;
import maths.vec3;

public class AnimObject extends Entity {
	public RenderQuadAnim quad;

	public AnimObject(float w, float h) {
		quad = new RenderQuadAnim(w, h);
	}
	
	public AnimObject(vec3 position, RenderQuadAnim quad) {
		this.position = position;
		this.quad = quad;
	}
	
	public AnimObject(float w, float h, String path) {
		quad = new RenderQuadAnim(w, h);
		
		quad.texture_array = new Texture[1];
		quad.texture_array[0] = new Texture();
		quad.texture_array[0].load_texture_file(path);
		quad.init();
	}
	
	public void render(ShaderProgram shader) {
        mat4 model_matrix = new mat4(1.0f);
        model_matrix.multiply(mat4.translate_matrix(position));
        
		shader.set_uniform("u_model_matrix", model_matrix);
		quad.render(shader);
	}
	
}
