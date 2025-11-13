package game;

import static org.lwjgl.glfw.GLFW.*;

import java.util.ArrayList;
import java.util.List;

import engine.CollisionBox;
import engine.Engine;
import engine.Input;
import engine.RenderQuad;
import engine.ShaderProgram;
import engine.Sound;
import engine.Texture;
import engine.Window;
import game.TriggerObject.TriggerType;
import maths.mat4;
import maths.vec3;

public class Level {
	public Player player;
	public int current_level;
	
	public StaticObject blocks[];
	public List<TriggerObject> object_list;
	public List<AnimObject> animated_list;

	public Sound dog_sound_array[];

	public RenderQuad suelo_array[];
	public AnimObject waterwall;
	public RenderQuad wall_displacement_array[];
	public RenderQuad wall_background_array[];
	public RenderQuad wall_decoration_array[];
	public RenderQuad quad_undefined;
	public RenderQuad dog;

	public RenderQuad carton_sprite;
	public RenderQuad llave_sprite;
	public RenderQuad alas_sprite;
	public RenderQuad monster_sprite;

	public enum ItemType {
		llave_type,
		carton_type,
		alas_type,
		monster_type,
		crayon_1_type,
		crayon_2_type,
		crayon_3_type,
		crayon_4_type,
	}
	
	public static final int EX = -2;
	public static final int PL = -1;
	public static final int BG = 0;
	
	public static final int WL = 1;
	public static final int WT = 2;
	public static final int GR = 7;
	
	public static final int WW = 3;
	public static final int LD = 4;
	public static final int SC = 5;
	public static final int DG = 6;
	
	public Level() {
		object_list 	= new ArrayList<>();
		animated_list 	= new ArrayList<AnimObject>();
		blocks 			= new StaticObject[25*25];
	}

	public vec3 get_position_from_index(int x, int y) {
		return(new vec3((float) x * 128.0f, (float) y * 128.0f, 0));
	}
	public vec3 get_position_from_index(float x, float y) {
		return(new vec3(x * 128.0f, y * 128.0f, 0));
	}
	
	public void load_layout(int[] map_layout, int col, int row) {
		blocks = new StaticObject[25*25];
		int background_index = current_level-1;
		if(background_index > wall_background_array.length-1) background_index = wall_background_array.length-1;
		
		for(int y= 0; y < row; y++) {
			for(int x = 0; x < col; x++) {
				int index = x + (((row-1) - y) * col);
				blocks[index] = new StaticObject();
				
				StaticObject it = blocks[index];
				it.block_type 	= map_layout[index];
				
				if(it.block_type == -1) {
					it.block_type = 0;
					player.position = get_position_from_index((float) x - 0.5f, y);
				}
				
				switch(it.block_type) {
					case 0: {
						it.position = get_position_from_index(x, y);
						it.quad 	= wall_background_array[background_index];
					} break;
					case EX: {
						it.position = get_position_from_index(x, y);
						it.quad 	= wall_background_array[background_index];
					} break;
					case WW: {
						it.position = get_position_from_index(x, y);
						it.quad 	= wall_background_array[background_index];
					} break;
					case WL: {
						it.position = get_position_from_index(x, y);
						it.quad 	= wall_displacement_array[Engine.random.nextInt(1)];
					} break;
					case GR: {
						it.position = get_position_from_index(x, y);
						it.quad 	= suelo_array[0];
					} break;
					case 2: {
						it.position = get_position_from_index(x, y);
						it.quad 	= wall_displacement_array[Engine.random.nextInt(1)];
					} break;
					case SC: {
						it.position = get_position_from_index(x, y);
						it.quad 	= wall_background_array[background_index];
					} break;
					case DG: {
						it.position = get_position_from_index(x, y);
						it.quad 	= dog;
					} break;
					default: {
						it.position = get_position_from_index(x, y);
						it.quad 	= quad_undefined;
					} 
				}
			}
		}
	}
	
	public void init() {
		player = new Player();
		player.init();

		waterwall = new AnimObject(253*0.5f, 512*0.5f);
		waterwall.quad.texture_array = new Texture[3];
		for(int i = 0; i < 3; i++) {
			waterwall.quad.texture_array[i] = new Texture();
			waterwall.quad.texture_array[i].load_texture_file("res/CASCADA_" + (i + 1) + ".png");
		}
		waterwall.quad.init();
		waterwall.quad.init_animation(253, 512, 253, 512, 3, 7);

		alas_sprite 	= new RenderQuad(64.0f, 64.0f, 1.0f, "res/alas.png");
		carton_sprite 	= new RenderQuad(64.0f, 64.0f, 1.0f, "res/carton.png");
		llave_sprite 	= new RenderQuad(64.0f, 64.0f, 1.0f, "res/llave.png");
		monster_sprite 	= new RenderQuad(64.0f, 64.0f, 1.0f, "res/monster.png");
		dog  			= new RenderQuad(128.0f, 128.0f, 1.0f, "res/raurrr_1.png");
		quad_undefined  = new RenderQuad(128.0f, 128.0f, 1.0f, "res/grass_1.png");
		
		wall_displacement_array = new RenderQuad[1];
		wall_displacement_array[0] = new RenderQuad(128.0f, 128.0f, 1.0f, "res/blocks/base.png");

		wall_background_array = new RenderQuad[4];
		for(int i = 0; i < wall_background_array.length; i++) {
			wall_background_array[i] = new RenderQuad(128.0f, 128.0f, 1.0f, "res/blocks/bg_" + (i + 1) + ".png");
		}
		
		suelo_array = new RenderQuad[1];
		suelo_array[0] = new RenderQuad(128.0f, 128.0f, 1.0f, "res/blocks/ground_1.png");
		/*
		wall_decoration_array = new RenderQuad[3];
		for(int i = 0; i < wall_decoration_array.length; i++) {
			wall_decoration_array[i] = new RenderQuad(128.0f, 128.0f, 1.0f, "res/blocks/bg_deco_" + (i + 1) + ".png");
		}
		*/
		dog_sound_array = new Sound[6];
		for(int i = 0; i < dog_sound_array.length; i++) {
			dog_sound_array[i] = new Sound();
			dog_sound_array[i].load_sound_from_file("res/sound/dog_" + (i + 1) + ".ogg", false);
		}
	}
	
	public void load_level_1() {
		animated_list.clear();
		object_list.clear();
		player.item_bag.clear();
		
		int[] map_layout = {
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, GR, GR, GR, GR, GR, GR, GR, BG, BG, BG, GR, GR, GR, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, PL, BG, BG, EX, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, BG, BG, WL, 
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, BG, BG, WL, 
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, BG, BG, WL, 
			//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  
		};
		
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(9.15f, 7.15f), 256.0f, 256.0f, "res/blocks/grafo_2.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(2.15f, 7.25f), 256.0f, 256.0f, "res/blocks/grafo_3.png"));

		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(10.15f, 4.15f), 256.0f, 256.0f, "res/blocks/grafo_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(2.15f, 4.15f), 256.0f, 256.0f,  "res/blocks/grafo_2.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(10.75f, 3.95f), 256.0f, 256.0f, "res/blocks/grafo_2.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(8.85f, 3.5f), 256.0f, 256.0f,   "res/blocks/grafo_3.png"));

		object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(1.25f, 7.25f), 64.0f, 64.0f,  "res/carton.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_crayon_1,  get_position_from_index(12.5f, 7.25f), 128.0f, 128.0f, "res/CRAYONES/yellow_4.png"));
		
		object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(13f, 3.25f), 64.0f, 64.0f,  "res/carton.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(8.5f, 3.25f), 64.0f, 64.0f, "res/carton.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_level,  get_position_from_index(4, 3), 937.0f * 0.35f, 768.0f * 0.35f, "res/door/puerta.png"));

		this.current_level 	= 1;
		load_layout(map_layout, 15, 10);
	}
	
	public void load_level_2() {
		animated_list.clear();
		object_list.clear();
		player.item_bag.clear();
		
		int[] map_layout = {
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, // 0
				WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, WL, WL, WL, WL, WL, // 1
				WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, WL, WL, WL, WL, WL, // 2
				WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, WL, WL, WL, WL, WL, // 3
				WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, WL, WL, WL, WL, WL, // 4
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, // 5
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, // 6
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, // 7
				WL, BG, BG, PL, BG, BG, BG, BG, BG, BG, BG, BG, EX, BG, WL, // 8
				WL, GR, GR, GR, GR, GR, BG, BG, BG, GR, GR, GR, GR, GR, WL, // 9
				WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, WL, WL, WL, WL, WL, // 10
				WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, WL, WL, WL, WL, WL, // 11
				WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, WL, WL, WL, WL, WL, // 12
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, // 13
			//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  
		};

		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(3.15f, 7.15f), 256.0f, 256.0f, "res/blocks/grafo_2.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(7.75f, 6.95f), 256.0f, 256.0f, "res/blocks/grafo_2.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(8.85f, 6.1f), 256.0f, 256.0f,   "res/blocks/grafo_3.png"));
		
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(6.25f, 9.15f), 256.0f, 256.0f,  "res/blocks/grafo_1.png"));

		object_list.add(new TriggerObject(TriggerType.trigger_carton, get_position_from_index(1.25f, 5.25f), 64.0f, 64.0f, "res/carton.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_carton, get_position_from_index(7.5f, 1.25f), 64.0f, 64.0f, "res/carton.png"));

		object_list.add(new TriggerObject(TriggerType.trigger_crayon_2, get_position_from_index(7.0f, 11.25f), 128.0f, 128.0f, "res/CRAYONES/green_4.png"));
		
		object_list.add(new TriggerObject(TriggerType.trigger_level,  get_position_from_index(12, 5), 937.0f * 0.35f, 768.0f * 0.35f, "res/door/puerta_abierta.png"));
		
		this.current_level = 2;
		load_layout(map_layout, 15, 14);
	}
	
	public void load_level_3() {
		object_list.clear();
		player.item_bag.clear();
		animated_list.clear();
		
		int[] map_layout = {
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, 
				WL, BG, BG, BG, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, 
				WL, BG, PL, BG, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, BG, BG, BG, BG, BG, BG, WL, WL, WL, WL, WL, WL, WL, WL, BG, BG, BG, BG, BG, BG, WL, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, BG, BG, BG, BG, BG, BG, WL, WL, WL, WL, WL, WL, WL, WL, BG, BG, BG, BG, BG, BG, WL, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, BG, BG, BG, BG, BG, BG, WL, WL, WL, WW, WL, WL, WL, WL, BG, BG, BG, BG, BG, BG, WL, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, GR, GR, BG, BG, BG, BG, BG, WW, BG, BG, BG, BG, BG, BG, GR, GR, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, WL, WL, BG, BG, BG, BG, BG, WW, BG, BG, BG, BG, BG, BG, WL, WL, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, WW, BG, GR, GR, BG, BG, WW, BG, BG, GR, GR, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, BG, BG, BG, WW, BG, WL, BG, BG, BG, WW, BG, BG, BG, WL, BG, BG, BG, BG, BG, BG, GR, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, GR, WT, WT, WT, GR, WL, BG, BG, BG, WW, BG, BG, BG, WL, GR, GR, WT, WT, WT, GR, WL, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, WW, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, WW, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, BG, EX, BG, BG, BG, BG, BG, BG, BG, WW, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, BG, BG, BG, WL, 
				WL, BG, BG, BG, WL, GR, GR, GR, GR, GR, GR, GR, BG, BG, WW, BG, BG, GR, GR, GR, GR, WT, WT, WT, GR, WL, BG, BG, BG, WL, 
			//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29
		};

		object_list.add(new TriggerObject(TriggerType.trigger_level,  get_position_from_index(6, 1), 937.0f * 0.35f, 768.0f * 0.35f, "res/door/puerta_abierta.png"));
		
		animated_list.add(new AnimObject(get_position_from_index(8, 5.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, 14.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, 12.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, 10.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, 8.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, 6.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, 4.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, 2.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, 0.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(14, -2.0f), waterwall.quad));
		
		object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(3.25f, 8.25f), 64.0f, 64.0f, "res/carton.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(1.25f, 6.25f), 64.0f, 64.0f, "res/carton.png"));
		
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(20, 4.65f), 460.0f, 60.0f, "res/fea_mancha_de_agua.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(6, 4.65f), 460.0f, 60.0f, "res/fea_mancha_de_agua.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(21, 0.65f), 460.0f, 60.0f, "res/fea_mancha_de_agua.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_crayon_3,  get_position_from_index(27.5f, 1.25f), 128.0f, 128.0f, "res/CRAYONES/violeta_4.png"));
		
		this.current_level = 3;
		load_layout(map_layout, 30, 15);
	}

	public void load_level_4() {
		animated_list.clear();
		object_list.clear();
		player.item_bag.clear();
		
		int[] map_layout = {
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, 
				WL, BG, BG, BG, BG, BG, WW, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, WW, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, WW, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, WW, BG, SC, BG, SC, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, WW, PL, SC, BG, SC, EX, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, SC, BG, SC, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, SC, BG, SC, BG, BG, BG, BG, BG, BG, WL, 
				WL, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, WL, 
			//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18 
		};

		object_list.add(new TriggerObject(TriggerType.trigger_dog,  get_position_from_index(10.0f, 1.0f), 128.0f, 128.0f, "res/raurrr_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_dog,  get_position_from_index(9.0f, 4.0f), 128.0f, 128.0f, "res/raurrr_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(4.25f, 4.25f), 64.0f, 64.0f, "res/alas.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(7.25f, 1.25f), 64.0f, 64.0f, "res/alas.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(14.25f, 1.25f), 64.0f, 64.0f, "res/alas.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(2.25F, 0.65f), 460.0f, 60.0f, "res/fea_mancha_de_agua.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(4.25F, 3.65f), 460.0f, 60.0f, "res/fea_mancha_de_agua.png"));

		animated_list.add(new AnimObject(get_position_from_index(6, 8.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(6, 6.0f), waterwall.quad));
		animated_list.add(new AnimObject(get_position_from_index(6, 4.0f), waterwall.quad));
		object_list.add(new TriggerObject(TriggerType.trigger_crayon_4, get_position_from_index(7.0f, 7.0f), 128.0f, 128.0f, "res/CRAYONES/rojo_4.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_level,  get_position_from_index(11, 4), 937.0f * 0.35f, 768.0f * 0.35f, "res/door/puerta.png"));

		this.current_level = 4;
		load_layout(map_layout, 19, 10);
	}

	public void load_level_true_end() {
		animated_list.clear();
		object_list.clear();
		player.item_bag.clear();
		
		int[] map_layout = {
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL,
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL,
				WL, BG, BG, BG, BG, BG, BG, PL, BG, BG, BG, BG, BG, BG, WL,
				WL, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, WL, 
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL,
			//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14 
		};

		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(2.5f, 4.5f), 256.0f, 256.0f, "res/blocks/grafo_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(5.0f, 4.5f), 256.0f, 256.0f, "res/blocks/grafo_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(7.5f, 4.5f), 256.0f, 256.0f, "res/blocks/grafo_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(10.0f, 4.5f), 256.0f, 256.0f, "res/blocks/grafo_2.png"));
		
		int crayon_count = 0;

		for(ItemType crayon : player.crayon_bag) {
			if(crayon == ItemType.crayon_4_type) {
				crayon_count += 1;
				object_list.add(new TriggerObject(TriggerType.trigger_crayon_3, get_position_from_index(10.5f, 5.0f), 128.0f, 128.0f, "res/CRAYONES/rojo_4.png"));
			}
		}

		if(crayon_count == 1) {
			for(int y = 0; y < 10; y++) {
				for(int x = 0; x < 15; x++) {
	
					int index = x + (((10-1) - y) * 15);
					int block_type = map_layout[index];
					
					if(block_type == BG) {
						object_list.add(new TriggerObject(TriggerType.trigger_carton,  get_position_from_index(x, y), 64.0f, 64.0f, "res/carton.png"));
					}
				}
			}
		}
		
		object_list.add(new TriggerObject(TriggerType.trigger_crayon_1, get_position_from_index(3.0f, 5.0f), 128.0f, 128.0f, "res/CRAYONES/yellow_4.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_crayon_2, get_position_from_index(5.5f, 5.0f), 128.0f, 128.0f, "res/CRAYONES/green_4.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_crayon_3, get_position_from_index(8.0f, 5.0f), 128.0f, 128.0f, "res/CRAYONES/rojo_4.png"));
		
		this.current_level = 6;
		load_layout(map_layout, 15, 10);
	}
	
	public void load_level_end() {
		animated_list.clear();
		object_list.clear();
		player.item_bag.clear();
		
		int[] map_layout = {
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL,
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL, 
				WL, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, WL,
				WL, BG, BG, BG, BG, BG, BG, PL, BG, BG, BG, BG, BG, BG, WL,
				WL, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, WL, 
				WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL,
			//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14 
		};

		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(2.5f, 4.5f), 256.0f, 256.0f, "res/blocks/grafo_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(5.0f, 4.5f), 256.0f, 256.0f, "res/blocks/grafo_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(7.5f, 4.5f), 256.0f, 256.0f, "res/blocks/grafo_1.png"));
		object_list.add(new TriggerObject(TriggerType.trigger_water, get_position_from_index(10.0f, 4.5f), 256.0f, 256.0f, "res/blocks/grafo_2.png"));
		
		int crayon_count = 0;

		for(ItemType crayon : player.crayon_bag) {
			if(crayon == ItemType.crayon_1_type) {
				crayon_count += 1;
				object_list.add(new TriggerObject(TriggerType.trigger_crayon_1, get_position_from_index(3.0f, 5.0f), 128.0f, 128.0f, "res/CRAYONES/yellow_4.png"));
			}
			if(crayon == ItemType.crayon_2_type) {
				crayon_count += 1;
				object_list.add(new TriggerObject(TriggerType.trigger_crayon_2, get_position_from_index(5.5f, 5.0f), 128.0f, 128.0f, "res/CRAYONES/green_4.png"));
			}
			if(crayon == ItemType.crayon_3_type) {
				crayon_count += 1;
				object_list.add(new TriggerObject(TriggerType.trigger_crayon_3, get_position_from_index(8.0f, 5.0f), 128.0f, 128.0f, "res/CRAYONES/rojo_4.png"));
			}
		}

		if(crayon_count == 3) {
			object_list.add(new TriggerObject(TriggerType.trigger_level,  get_position_from_index(11, 2), 937.0f * 0.35f, 768.0f * 0.35f, "res/door/puerta_abierta.png"));
		} else {
			object_list.add(new TriggerObject(TriggerType.trigger_level,  get_position_from_index(11, 2), 937.0f * 0.35f, 768.0f * 0.35f, "res/door/puerta.png"));
		}
		
		this.current_level = 5;
		load_layout(map_layout, 15, 10);
	}
	
	public void input(Window window, Input input, float delta_time) {
		/*
		if(window.is_key_typed(GLFW_KEY_1) && !player.item_bag.isEmpty()) {
			ItemType new_type = player.item_bag.get(0).equals(ItemType.carton_type) ? ItemType.llave_type : ItemType.carton_type;
			player.item_bag.set(0, new_type);
		}
		*/
		
		if(window.is_key_typed(GLFW_KEY_1) && !player.item_bag.isEmpty()) {
			for(int i = 0; i < player.item_bag.size(); i++) {
				ItemType it = player.item_bag.get(i);
				
				if(it.equals(ItemType.carton_type)/* || it.equals(ItemType.alas_type)*/) {
					player.item_bag.set(i, ItemType.llave_type);
					break;
				}
			}
		}
		
		if(window.is_key_typed(GLFW_KEY_2)) {
			
			int carton_count = 0;
			for(int i = 0; i < player.item_bag.size(); i++) {
				ItemType it = player.item_bag.get(i);
				if(it == ItemType.carton_type) {
					 carton_count += 1;
				}
			}
			
			if(carton_count >= 2) {
				player.item_bag.remove(ItemType.carton_type);
				player.item_bag.remove(ItemType.carton_type);
				player.item_bag.add(ItemType.alas_type);
			}
			
		}
		
		/*if(window.is_key_typed(GLFW_KEY_3) && !player.item_bag.isEmpty()) {
			for(int i = 0; i < player.item_bag.size(); i++) {
				ItemType it = player.item_bag.get(i);
				
				if(it.equals(ItemType.carton_type)) {
					player.item_bag.set(i, ItemType.monster_type);
					break;
				}
			}
		}*/
		
		switch(current_level) {
			case 1: {
				if(window.is_key_typed(GLFW_KEY_R)) load_level_1();
			} break;
			case 2: {
				if(window.is_key_typed(GLFW_KEY_R)) load_level_2();
			} break;
			case 3: {
				if(window.is_key_typed(GLFW_KEY_R)) load_level_3();
			} break;
			case 4: {
				if(window.is_key_typed(GLFW_KEY_R)) load_level_4();
			} break;
			case 5: {
				if(window.is_key_typed(GLFW_KEY_R)) load_level_1();
			} break;
		}
		
		
		player.input(window, delta_time);
	}
	
	public void render(Window window, ShaderProgram shader) {
		for(StaticObject it : blocks) {
			if(it != null) {
				it.render(shader);
			}
		}
		
		for(TriggerObject it : object_list) {
			if(!it.removed) 
				it.render(shader);
		}

		player.render(shader);

		for(AnimObject it : animated_list) {
			it.render(shader);
		}
		
		mat4 u_view_matrix = new mat4(1.0f);
		shader.set_uniform("u_view_matrix", u_view_matrix);
		
		int slot = 0;
		for(ItemType item : player.item_bag) {
			RenderQuad icon = carton_sprite;
			
			switch(item) {
			case carton_type:
				icon = carton_sprite;
				break;
			case llave_type:
				icon = llave_sprite;
				break;
			case alas_type:
				icon = alas_sprite;
				break;
			case monster_type:
				icon = monster_sprite;
				break;
			}
			
			float width_2 = window.width * 0.5f;
			float pos_x = (slot * (icon.rect.w * 1.0f));
			vec3 position = new vec3((slot * (icon.rect.w * 1.0f)) + window.width * -0.5f, window.height * -0.5f, 0);
			if(pos_x >= window.width) {
				double times = Math.ceil(pos_x / window.width);
				System.out.println(window.width +" - " + times);
				position.x = position.x - (width_2 * (float) times);
				position.y += times * (icon.rect.h * 0.5f);
			}
			
			mat4 model_matrix = new mat4(1.0f);
			
			model_matrix.multiply(mat4.translate_matrix(position));
			shader.set_uniform("u_model_matrix", model_matrix);
			
			icon.render(shader);
			slot += 1;
		}
	}

	public void update(Window window, float delta_time) {
		CollisionBox player_box = player.get_box();
		boolean end_level = false;
		
		for(TriggerObject it : object_list) {
			if(!it.removed && it.box.do_overlap(player_box)) {
				switch(it.type) {
					case trigger_carton: {
						it.removed = true;
						player.item_bag.add(ItemType.carton_type);
					} break;
					case trigger_alas: {
						it.removed = true;
						player.item_bag.add(ItemType.alas_type);
					} break;
					case trigger_water: { 
					} break;
					case trigger_level: {
						end_level = true;
					} break;
				case trigger_crayon_1:
					it.removed = true;
					player.crayon_bag.add(ItemType.crayon_1_type);
					break;
				case trigger_crayon_2:
					it.removed = true;
					player.crayon_bag.add(ItemType.crayon_2_type);
					break;
				case trigger_crayon_3:
					it.removed = true;
					player.crayon_bag.add(ItemType.crayon_3_type);
					break;
				case trigger_crayon_4:
					it.removed = true;
					player.crayon_bag.add(ItemType.crayon_4_type);
					break;
				default:
					break;
				}
			}
		}
		
		if(end_level) {
			handle_level_end();
		}

		waterwall.quad.update(delta_time);
		
		player.update(window, this, delta_time);
	}

	public void handle_level_end() {
		switch(current_level) {
			case 1: {
				for(int i = 0; i < player.item_bag.size(); i++) {
					ItemType it = player.item_bag.get(i);
					if(it.equals(ItemType.llave_type)) {
						load_level_2();
					}
				}
			} break;
			case 2: {
				load_level_3();
			} break;
			case 3: {
				load_level_end();
			} break;
			case 4: {
				for(int i = 0; i < player.item_bag.size(); i++) {
					ItemType it = player.item_bag.get(i);
					if(it.equals(ItemType.llave_type)) {
						load_level_true_end();
					}
				}
			} break;
			case 5: {
				int crayon_count = 0;

				for(ItemType crayon : player.crayon_bag) {
					if(crayon == ItemType.crayon_1_type || crayon == ItemType.crayon_2_type || crayon == ItemType.crayon_3_type) {
						crayon_count += 1;
					}
				}
				if(crayon_count == 3) {
					load_level_4();
				}
			} break;
		}
	}
	
}
