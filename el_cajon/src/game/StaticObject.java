package game;

import engine.RenderQuad;
import engine.ShaderProgram;
import maths.mat4;
import maths.vec3;

public class StaticObject extends Entity {
	public RenderQuad quad;
	public int block_type;

	public StaticObject() {
		position = new vec3(0, 0, 0);
	}
	
	public void render(ShaderProgram shader) {
        mat4 model_matrix = new mat4(1.0f);
        model_matrix.multiply(mat4.translate_matrix(position));
        
		shader.set_uniform("u_model_matrix", model_matrix);
		quad.render(shader);
	}
	
}
