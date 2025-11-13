package game;

import engine.CollisionBox;
import engine.RenderQuad;
import engine.ShaderProgram;
import maths.mat4;
import maths.vec3;

public class TriggerObject extends Entity {
	public RenderQuad quad;
	public CollisionBox box;
	public boolean removed;
	public TriggerType type;
	
	public enum TriggerType {
		trigger_water,
		trigger_carton,
		trigger_level,
		trigger_alas,
		trigger_dog,
		
		trigger_crayon_1,
		trigger_crayon_2,
		trigger_crayon_3,
		trigger_crayon_4,
	}
	
	public TriggerObject(TriggerType type, vec3 position, float width, float height, String path) {
		this.type = type;
		this.position = position;
		
		quad = new RenderQuad(width, height, 1.0f, path);
		box  = new CollisionBox(position.x, position.y, position.x + width * 1.0f, position.y + height * 1.0f);
	}

	public void render(ShaderProgram shader) {
        mat4 model_matrix = new mat4(1.0f);
        model_matrix.multiply(mat4.translate_matrix(position));
        
		shader.set_uniform("u_model_matrix", model_matrix);
		quad.render(shader);
	}
	
}
